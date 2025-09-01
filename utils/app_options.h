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

    ~AppOptions();

    void actionListenerCallback (const juce::String& message) override;

    void setOption (const juce::Identifier& identifier, juce::var value);

    const juce::var operator[] (const juce::Identifier& identifier) const;

    void save();

    void load();

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
        virtual void optionsChangedEarlyCallback (const juce::Identifier& /* identifierThatChanged */){};
        virtual void optionsChanged (const juce::Identifier& identifierThatChanged) = 0;
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
    bool preventTriggeringSave{};

    class ThreadSafeValueProxy;

    std::unique_ptr<juce::InterProcessLock> lock;

    juce::ListenerList<Listener, juce::Array<Listener*, juce::CriticalSection>> listeners;
    int suppressCallback{ 0 };

    std::unique_ptr<ThreadSafeValueProxy> valueProxy;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppOptions)
};

} // namespace jcf
