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

	class Listener
	{
	public:
		virtual ~Listener();
		/** Is called before optionsChanged in case you need to do some early work! */
		virtual void optionsChangedEarlyCallback(const Identifier & identifierThatChanged) {};
		virtual void optionsChanged(const Identifier & identifierThatChanged) = 0;
	};

	void addListener(Listener* listener);
	void removeListener(Listener* listener);

	/** You can attach data directly to the state tree, but the propertyChanged callback might not work properly */
	ValueTree state;
private:
	void triggerTimer();

	void timerCallback() override;

	void valueTreePropertyChanged(ValueTree&, const Identifier& identifier) override;

	void valueTreeChildAdded(ValueTree&, ValueTree&) override;
	void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override;
	void valueTreeChildOrderChanged(ValueTree&, int, int) override;
	void valueTreeParentChanged(ValueTree&) override;

	File file;
	std::set<Identifier> identifiersThatChanged;
	ScopedPointer<InterProcessLock> lock;
	ListenerList<Listener> listeners;
	int suppressCallback{ 0 };

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppOptions)
};


