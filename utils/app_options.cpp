jcf::AppOptions::AppOptions(const File& file): file(file)
{
	lock = new InterProcessLock(file.getFullPathName());
    state = ValueTree{ "state" };
    state.addListener(this);
	load();
	MessageManager::getInstance()->registerBroadcastListener(this);
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
	auto currentValue = operator[](identifier);

	if (! value.equals(currentValue))
		state.setProperty(identifier, value, nullptr);
}

const juce::var jcf::AppOptions::operator[](const Identifier& identifier) const
{
	return state[identifier];
}

void jcf::AppOptions::save()
{
	{
		InterProcessLock::ScopedLockType l(*lock);
		jcf::saveValueTreeToXml(file, state);
	}

	suppressCallback++;

	MessageManager::getInstance()->broadcastMessage(file.getFullPathName());
}

void jcf::AppOptions::load()
{
	InterProcessLock::ScopedLockType l(*lock);

	auto newState = jcf::loadValueTreeFromXml(file);

    if (newState == ValueTree::invalid)
        return;

    state.copyPropertiesFrom(newState, nullptr);
}

juce::Value jcf::AppOptions::getValueObject(const Identifier& identifier)
{
	return state.getPropertyAsValue(identifier, nullptr);
}

void jcf::AppOptions::setDefault(const Identifier& identifier, var defaultValue)
{
	if (!state.hasProperty(identifier))
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
	startTimer(1000);
}

void jcf::AppOptions::timerCallback()
{
	stopTimer(); // in case we get a modal loop in listeners.call

	save();

	for (auto & i : identifiersThatChanged)
		listeners.call(&Listener::optionsChangedEarlyCallback, i);

	for (auto & i : identifiersThatChanged)
		listeners.call(&Listener::optionsChanged, i);

	identifiersThatChanged.clear();
}

void jcf::AppOptions::valueTreePropertyChanged(ValueTree&, const Identifier& identifier)
{
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
