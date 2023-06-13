#pragma once

/**
A ValueTree based alternative to the JUCE PropertiesFile.
*/
class AppOptions
	:
	public ValueTree::Listener,
	Timer,
	ActionListener
{
public:
	explicit AppOptions(const File& file);

	~AppOptions();

	void actionListenerCallback(const String& message) override;

	void setOption(const Identifier& identifier, var value);

	const var operator[](const Identifier& identifier) const;

	void save();

	void load();

	Value getValueObject(const Identifier& identifier);

	/**
	Sets an option if it doesn't already have a value in the configuration file.
	*/
	void setDefault(const Identifier& identifier, var defaultValue);

	/**
	 * used for when the list of allowable options may have changed.
	 * applies the given default value if the option is set to a value that is not in the permitted list
	 * Note, this ignores the type of the current option value so a possible permittedList value of "123" will allow an option value of int(123)
	 */
	void setDefaultAndRestrictToPermittedList(const Identifier& identifier, const Array<var>& permittedList, var defaultValue);
	
	class Listener
	{
	public:
		virtual ~Listener();
		/** Is called before optionsChanged in case you need to do some early work! */
		virtual void optionsChangedEarlyCallback(const Identifier & /* identifierThatChanged */) {};
		virtual void optionsChanged(const Identifier & identifierThatChanged) = 0;
	};

	void addListener(Listener* listener);
	void removeListener(Listener* listener);

private:
    /** This was formerly public, but there's a massive issue with loading
     * preferences if we have Value objects based on properties. */
	ValueTree state;
	CriticalSection stateLock;

	void triggerTimer();

	void timerCallback() override;

	void valueTreePropertyChanged(ValueTree&, const Identifier& identifier) override;

	void valueTreeChildAdded(ValueTree&, ValueTree&) override;
	void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override;
	void valueTreeChildOrderChanged(ValueTree&, int, int) override;
	void valueTreeParentChanged(ValueTree&) override;

	File file;
	std::set<Identifier> identifiersThatChanged;
    bool preventTriggeringSave{};


	class ThreadSafeValueProxy;
    //ScopedPointer<InterProcessLock> lock;


	std::unique_ptr<InterProcessLock> lock;

	ListenerList<Listener, Array<Listener*, CriticalSection>> listeners;
	int suppressCallback{ 0 };

	std::unique_ptr<ThreadSafeValueProxy> valueProxy;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppOptions)
};


