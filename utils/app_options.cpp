
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

	void optionsChanged(const Identifier &identifierThatChanged) override
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

jcf::AppOptions::AppOptions(const File& file): file(file)
{
	//lock = new InterProcessLock(file.getFullPathName());
	lock = std::make_unique<InterProcessLock>(file.getFullPathName());
	state = ValueTree{ "state" };
	load();
	MessageManager::getInstance()->registerBroadcastListener(this);
	state.addListener(this);

	valueProxy = std::make_unique<ThreadSafeValueProxy>(*this);
}

jcf::AppOptions::~AppOptions()
{
	save();

	if (MessageManager::getInstanceWithoutCreating())
		MessageManager::getInstanceWithoutCreating()->deregisterBroadcastListener(this);
}

void jcf::AppOptions::actionListenerCallback(const String& message)
{
	if (message == file.getFullPathName() && suppressCallback-- > 0)
		load();
}

void jcf::AppOptions::setOption(const Identifier& identifier, var value)
{
	ScopedLock lock{ stateLock };

	auto currentValue = operator[](identifier);

	if (! state.hasProperty(identifier) || ! value.equals(currentValue))
		state.setProperty(identifier, value, nullptr);
}

const juce::var jcf::AppOptions::operator[](const Identifier& identifier) const
{
	ScopedLock lock{ stateLock };

	return state[identifier];
}

void jcf::AppOptions::save()
{
    DBG("jcf::AppOptions::save()");

	{
		InterProcessLock::ScopedLockType l(*lock);
		ScopedLock l0{ stateLock };
		jcf::saveValueTreeToXml(file, state);
	}

	suppressCallback++;

    if (MessageManager::getInstanceWithoutCreating() != nullptr) // check to avoid a barely comprehensible crash on some shutdowns
        MessageManager::broadcastMessage(file.getFullPathName());
}

void jcf::AppOptions::load()
{

	DBG("jcf::AppOptions::load()");

	InterProcessLock::ScopedLockType l(*lock);

	auto newState = jcf::loadValueTreeFromXml(file);

    if (! newState.isValid())
        return;

    {
        ScopedLock lock{ stateLock };
        preventTriggeringSave = true;
        state.copyPropertiesFrom (newState, nullptr);
        preventTriggeringSave = false;
    }
}

juce::Value jcf::AppOptions::getValueObject(const Identifier& identifier)
{
	JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED
	return valueProxy->getOrCreateValueObject (identifier);
}

void jcf::AppOptions::setDefault(const Identifier& identifier, var defaultValue)
{
	ScopedLock lock{ stateLock };

	if (!state.hasProperty(identifier))
		setOption(identifier, defaultValue);
}

void jcf::AppOptions::setDefaultAndRestrictToPermittedList(const Identifier& identifier, const Array<var>& permittedList, var defaultValue)
{
	ScopedLock lock{ stateLock };

	if (!state.hasProperty(identifier))
	{
		setOption(identifier, defaultValue);
		return;
	}

	const auto currentValue = state.getProperty(identifier);

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

void jcf::AppOptions::addListener(Listener* listener)
{
	listeners.add(listener);
}

void jcf::AppOptions::removeListener(Listener* listener)
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
		ScopedLock lock{ stateLock };
		copyOfIds = identifiersThatChanged;
		identifiersThatChanged.clear();
	}

 	DBG("jcf::AppOptions::timerCallback()");
	stopTimer(); // in case we get a modal loop in listeners.call

	save();

	for (auto & i : copyOfIds)
		listeners.call(&Listener::optionsChangedEarlyCallback, i);

	for (auto & i : copyOfIds)
		listeners.call(&Listener::optionsChanged, i);
}

void jcf::AppOptions::valueTreePropertyChanged(ValueTree&, const Identifier& identifier)
{
	ScopedLock lock{ stateLock };

	DBG("jcf::AppOptions::valueTreePropertyChanged() " + identifier);
	triggerTimer();
	identifiersThatChanged.insert(identifier);
}

void jcf::AppOptions::valueTreeChildAdded(ValueTree&, ValueTree&)
{
	triggerTimer();
}

void jcf::AppOptions::valueTreeChildRemoved(ValueTree&, ValueTree&, int)
{
	triggerTimer();
}

void jcf::AppOptions::valueTreeChildOrderChanged(ValueTree&, int, int)
{
	triggerTimer();
}

void jcf::AppOptions::valueTreeParentChanged(ValueTree&)
{
	triggerTimer();
} 
