#pragma once
#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

namespace jcf
{

/**
 * A ValueTree based alternative to the JUCE PropertiesFile for saving your application's
 * options.
 */
class AppOptions : public juce::ValueTree::Listener, juce::Timer, juce::ActionListener
{
public:
	explicit AppOptions(const juce::File& file, bool readonly = false);

    ~AppOptions() override;

    /** Receives cross-process broadcast messages. Reloads options from disk
     *  when another process saves; skips reload for our own broadcasts. 
     */
    void actionListenerCallback (const juce::String& message) override;

    void setOption (const juce::Identifier& identifier, juce::var value);

    const juce::var operator[] (const juce::Identifier& identifier) const;

    void save();

    void load();

    /** Schedules a reload from disk, coalescing rapid calls via RateLimitedCallback. */
    void loadRateLimited();

    /** Suppresses broadcastMessage in save(). Call after wiring up an alternative
     *  cross-process notification mechanism (e.g. a notification bus). */
    void disableBroadcastMessage();

    juce::Value getValueObject (const juce::Identifier& identifier);

    /**
     * Sets an option if it doesn't already have a value in the configuration file.
     */
    void setDefault (const juce::Identifier& identifier, juce::var defaultValue);

    /**
     * Used for when the list of allowable options may have changed. Applies the given
     * default value if the option is set to a value that is not in the permitted list
     * Note, this ignores the type of the current option value so a possible permittedList
     * value of "123" will allow an option value of int(123)
     */
    void setDefaultAndRestrictToPermittedList (const juce::Identifier& identifier,
                                               const juce::Array<juce::var>& permittedList,
                                               juce::var defaultValue);

    class Listener
    {
    public:
        virtual ~Listener();
        /** Is called before optionsChanged in case you need to do some early work! */
        virtual void optionsChangedEarlyCallback (const juce::Identifier& /* identifierThatChanged */) {}
        virtual void optionsChanged (const juce::Identifier& identifierThatChanged) = 0;

        /** Called once after save() when options were changed locally (by this process).
         *  Not called during load() — only for changes originating here.
         *  Use this to notify other processes that options have changed. */
        virtual void optionsChangedLocally() {}
    };

    void addListener (Listener* listener);
    void removeListener (Listener* listener);

private:
    /** This was formerly public, but there's a massive issue with loading
     * preferences if we have Value objects based on properties. */
    juce::ValueTree state;
    juce::CriticalSection stateLock;

    void triggerTimer();

    void timerCallback() override;

    void valueTreePropertyChanged (juce::ValueTree&, const juce::Identifier& identifier) override;

    void valueTreeChildAdded (juce::ValueTree&, juce::ValueTree&) override;
    void valueTreeChildRemoved (juce::ValueTree&, juce::ValueTree&, int) override;
    void valueTreeChildOrderChanged (juce::ValueTree&, int, int) override;
    void valueTreeParentChanged (juce::ValueTree&) override;

    bool readOnly{ false };
    juce::File file;
    std::set<juce::Identifier> identifiersThatChanged;

    /** Settings this process has changed and not yet written. save() writes these over
        whatever is in the file, leaving everything else in the file alone, so a save does
        not undo a change another process made while we were holding an older copy. Cleared
        by save(). Changes that arrived from the file in load() are not counted. */
    std::set<juce::Identifier> identifiersChangedSinceSave;

    bool preventTriggeringSave{};

    class ThreadSafeValueProxy;

    std::unique_ptr<juce::InterProcessLock> lock;

    juce::ListenerList<Listener, juce::Array<Listener*, juce::CriticalSection>> listeners;
    /** Counts pending own-broadcast messages to suppress. Incremented in save(),
     *  decremented when our own broadcast is received back. */
    int suppressCallback{ 0 };

    std::unique_ptr<ThreadSafeValueProxy> valueProxy;

    bool broadcastDisabled{ false };
    std::unique_ptr<RateLimitedCallback> debouncedLoad;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppOptions)
};

} // namespace jcf
