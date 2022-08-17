jcf::AppOptions::AppOptions(const File& file): file(file)
{
	//lock = new InterProcessLock(file.getFullPathName());
	lock = std::make_unique<InterProcessLock>(file.getFullPathName());
    state = ValueTree{ "state" };
	load();
	MessageManager::getInstance()->registerBroadcastListener(this);
    state.addListener(this);
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

	if (! state.hasProperty(identifier) || ! value.equals(currentValue))
		state.setProperty(identifier, value, nullptr);
}

const juce::var jcf::AppOptions::operator[](const Identifier& identifier) const
{
	return state[identifier];
}

void jcf::AppOptions::save()
{
    DBG("jcf::AppOptions::save()");
	{
		InterProcessLock::ScopedLockType l(*lock);
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

void jcf::AppOptions::setDefaultAndRestrictToPermittedList(const Identifier& identifier, const Array<var>& permittedList, var defaultValue)
{
	if (!state.hasProperty(identifier))
	{
		setOption(identifier, defaultValue);
		return;
	}

	const auto currentValue = state.getProperty(identifier);
	
	for(auto option: permittedList)
	{
		if(currentValue.equals (option))
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
	startTimer(1000);
}

void jcf::AppOptions::timerCallback()
{
    DBG("jcf::AppOptions::timerCallback()");
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
