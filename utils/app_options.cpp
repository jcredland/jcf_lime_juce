
#include "app_options.h"
namespace jcf
{
/**
 * Allows use to provide Value objects for use on the message thread without invaliding our threadsafety by
 * modifying the ValueTree on the message thread.
 */
class AppOptions::ThreadSafeValueProxy : public Value::Listener, public AppOptions::Listener
{
public:
    ThreadSafeValueProxy (AppOptions& appOptions_) : appOptions (appOptions_) { appOptions.addListener (this); }

    ~ThreadSafeValueProxy() override { appOptions.removeListener (this); }

    Value getOrCreateValueObject (const Identifier& id)
    {
        JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

        for (auto& x : values)
        {
            if (x->id == id)
                return x->value;
        }

        // we didn't find the value in our list

        auto obj = std::make_unique<Record> (id, Value (appOptions[id]));
        obj->value.addListener (this);
        values.push_back (std::move (obj));

        return values.back()->value;
    }

    void optionsChanged (const Identifier& identifierThatChanged) override
    {
        for (auto& x : values)
        {
            if (x->id == identifierThatChanged)
            {
                x->value = appOptions[identifierThatChanged];
                return;
            }
        }
    }

    void valueChanged (Value& value) override
    {
        for (auto& x : values)
        {
            if (x->value.refersToSameSourceAs (value))
            {
                appOptions.setOption (x->id, value.getValue());
                return;
            }
        }

        // we didn't find the value in our list
        jassertfalse;
    }

    struct Record
    {
        Record (Identifier id_, Value value_) : id (id_), value (value_) {}
        Identifier id;
        Value value;
    };

    std::vector<std::unique_ptr<Record>> values;
    AppOptions& appOptions;
};

jcf::AppOptions::AppOptions(const File& f, bool isReadOnly): readOnly (isReadOnly), file(f)
{
    // lock = new InterProcessLock(file.getFullPathName());
    lock = std::make_unique<InterProcessLock> (file.getFullPathName());
    state = ValueTree{ "state" };
    load();
    MessageManager::getInstance()->registerBroadcastListener (this);
    state.addListener (this);

    valueProxy = std::make_unique<ThreadSafeValueProxy> (*this);
    debouncedLoad = std::make_unique<RateLimitedCallback> ([this]() { load(); }, 200);
}

jcf::AppOptions::~AppOptions()
{
    save();

    if (MessageManager::getInstanceWithoutCreating())
        MessageManager::getInstanceWithoutCreating()->deregisterBroadcastListener (this);
}

void jcf::AppOptions::actionListenerCallback(const String& message)
{
    if (message != file.getFullPathName())
        return;

    if (suppressCallback > 0)
    {
        suppressCallback--;
        return;  // We sent this broadcast (our own save), skip reload
    }

    load();  // Another process saved, reload
}

void jcf::AppOptions::setOption(const Identifier& identifier, var value)
{
    ScopedLock sl{ stateLock };

    auto currentValue = operator[] (identifier);

    if (! state.hasProperty (identifier) || ! value.equals (currentValue))
        state.setProperty (identifier, value, nullptr);
}

const juce::var jcf::AppOptions::operator[](const Identifier& identifier) const
{
    ScopedLock sl{ stateLock };

    return state[identifier];
}

void jcf::AppOptions::save()
{
    if (readOnly)
    {
        DBG("jcf::AppOptions::save() skipped - readonly");
        return;
    }

    DBG("jcf::AppOptions::save()");

    {
        InterProcessLock::ScopedLockType l (*lock);
        ScopedLock l0{ stateLock };

        // Write our own changes over what is in the file rather than replacing the lot. Another
        // process may have changed a setting since we last read, and our copy of that setting is
        // then out of date - writing the whole tree would put the old value back.
        //
        // Note we deliberately do not adopt what we read into state. That would fire listeners
        // from inside a save, and save() is called from the destructor where the message thread
        // may be gone. Reading fresh values is what load() and the change notifications are for.
        auto onDisk = jcf::loadValueTreeFromXml (file);

        if (onDisk.isValid())
        {
            for (const auto& identifier : identifiersChangedSinceSave)
                onDisk.setProperty (identifier, state[identifier], nullptr);

            jcf::saveValueTreeToXml (file, onDisk);
        }
        else
        {
            // No readable file yet - ours is the only copy there is.
            jcf::saveValueTreeToXml (file, state);
        }

        identifiersChangedSinceSave.clear();
    }

    if (! broadcastDisabled)
    {
        suppressCallback++;

        if (MessageManager::getInstanceWithoutCreating() != nullptr) // check to avoid a barely comprehensible crash on some shutdowns
            MessageManager::broadcastMessage (file.getFullPathName());
    }
}

void jcf::AppOptions::load()
{
    DBG("jcf::AppOptions::load()");
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED

    InterProcessLock::ScopedLockType l (*lock);

    auto newState = jcf::loadValueTreeFromXml (file);

    if (! newState.isValid())
        return;

    {
        ScopedLock sl{ stateLock };
        preventTriggeringSave = true;
        state.copyPropertiesFrom (newState, nullptr);
        preventTriggeringSave = false;
    }

    // Drain identifiers accumulated during copyPropertiesFrom and notify
    // listeners directly. We bypass the timer to avoid timerCallback calling
    // save() and re-broadcasting, which would cause an infinite cascade.
    std::set<Identifier> changedIds;
    {
        ScopedLock lock{ stateLock };
        changedIds = std::move(identifiersThatChanged);
        identifiersThatChanged.clear();
    }

    for (auto& i : changedIds)
        listeners.call (&Listener::optionsChangedEarlyCallback, i);
    
    for (auto& i : changedIds)
        listeners.call (&Listener::optionsChanged, i);
}

juce::Value jcf::AppOptions::getValueObject(const Identifier& identifier)
{
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
    return valueProxy->getOrCreateValueObject (identifier);
}

void jcf::AppOptions::setDefault(const Identifier& identifier, var defaultValue)
{
    ScopedLock sl{ stateLock };

    if (! state.hasProperty (identifier))
        setOption (identifier, defaultValue);
}

void jcf::AppOptions::setDefaultAndRestrictToPermittedList(const Identifier& identifier, const Array<var>& permittedList, var defaultValue)
{
    ScopedLock sl{ stateLock };

    if (! state.hasProperty (identifier))
    {
        setOption (identifier, defaultValue);
        return;
    }

    const auto currentValue = state.getProperty (identifier);

    for (auto option : permittedList)
    {
        if (currentValue.equals (option))
        {
            // we match one of the possible options, potentially with a different type e.g "123"==123
            return;
        }
    }

    setOption(identifier, defaultValue);
}

jcf::AppOptions::Listener::~Listener() = default;

void jcf::AppOptions::addListener (Listener* listener)
{
    listeners.add(listener);
}

void jcf::AppOptions::removeListener (Listener* listener)
{
    listeners.remove(listener);
}

void jcf::AppOptions::triggerTimer()
{
    if (! preventTriggeringSave)
        startTimer(1000);
}

void jcf::AppOptions::timerCallback()
{
    std::set<Identifier> copyOfIds;

    {
        ScopedLock sl{ stateLock };
        copyOfIds = identifiersThatChanged;
        identifiersThatChanged.clear();
    }

    DBG ("jcf::AppOptions::timerCallback()");
    stopTimer(); // in case we get a modal loop in listeners.call

    save();

    for (auto& i : copyOfIds)
        listeners.call (&Listener::optionsChangedEarlyCallback, i);

    for (auto& i : copyOfIds)
        listeners.call (&Listener::optionsChanged, i);

    listeners.call (&Listener::optionsChangedLocally);
}

void jcf::AppOptions::loadRateLimited()
{
    debouncedLoad->trigger();
}

void jcf::AppOptions::disableBroadcastMessage()
{
    broadcastDisabled = true;
}

void jcf::AppOptions::valueTreePropertyChanged(ValueTree&, const Identifier& identifier)
{
    ScopedLock sl{ stateLock };

    DBG ("jcf::AppOptions::valueTreePropertyChanged() " + identifier);
    triggerTimer();
    identifiersThatChanged.insert(identifier);

    // preventTriggeringSave is set while load() copies the file into state, so this tells a
    // change we made apart from one we just read. Only ours should be written back.
    if (! preventTriggeringSave)
        identifiersChangedSinceSave.insert (identifier);
}

void jcf::AppOptions::valueTreeChildAdded (ValueTree&, ValueTree&)
{
    triggerTimer();
}

void jcf::AppOptions::valueTreeChildRemoved(ValueTree&, ValueTree&, int)
{
    triggerTimer();
}

void jcf::AppOptions::valueTreeChildOrderChanged (ValueTree&, int, int)
{
    triggerTimer();
}

void jcf::AppOptions::valueTreeParentChanged (ValueTree&)
{
    triggerTimer();
}
}