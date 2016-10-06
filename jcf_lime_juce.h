#pragma once

/*
 
 BEGIN_JUCE_MODULE_DECLARATION

  ID:               jcf_lime_juce
  vendor:           juce
  version:          4.2.1
  name:             JCF LIME JUCE
  description:      Utilities applicable to many JUCE application
  website:          http://www.juce.com/juce
  license:          Commerical

  dependencies:     juce_core juce_data_structures
  OSXFrameworks:    
  iOSFrameworks:    

 END_JUCE_MODULE_DECLARATION
 */

#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_cryptography/juce_cryptography.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <random>
#include <vector>
#include <set>

namespace juce
{
	inline bool operator<(const Identifier &a, const Identifier & b)
	{
		return a.toString() < b.toString();
	}
}

namespace jcf
{

#define log_Ctor {std::stringstream s; s << std::hex << int64(this) << " constructed " << typeid(*this).name() << std::endl; DBG(s.str());}
#define log_Dtor {std::stringstream s; s << std::hex << int64(this) << " deleted " << typeid(*this).name() << std::endl; DBG(s.str());}

    using namespace juce;

	template <typename ComponentType>
	void addAndMakeVisibleComponent(Component * parent, ComponentType & comp)
	{
		parent->addAndMakeVisible(comp);
	}

	template<typename ComponentType, typename... Args>
	void addAndMakeVisibleComponent(Component * parent, ComponentType & comp, Args&... args)
	{
		parent->addAndMakeVisible(comp);

		addAndMakeVisibleComponent(parent, args...);
	}

    /**
     Loads a valuetree compatible XML file. Returna ValueTree::invalid if something goes wrong.
     */
    inline ValueTree loadValueTreeFromXml(const File & file)
    {
        ScopedPointer<XmlElement> xml = XmlDocument(file.loadFileAsString()).getDocumentElement();

        if (!xml)
            return ValueTree::invalid;

        auto tree = ValueTree::fromXml(*xml);
        return tree;
    }

    inline Result saveValueTreeToXml(const File & file, const ValueTree & tree)
    {
        auto string = tree.toXmlString();
        auto result = file.replaceWithText(string);

        if (!result)
            return Result::fail("Save failed to " + file.getFullPathName());

        return Result::ok();
    }

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
		explicit AppOptions(const File & file) : file(file)
		{
			lock = new InterProcessLock(file.getFullPathName());
			load();
			MessageManager::getInstance()->registerBroadcastListener(this);
		}

		~AppOptions()
		{
			save();
			MessageManager::getInstance()->deregisterBroadcastListener(this);
		}

		void actionListenerCallback(const String& message) override
		{
			if (message == file.getFullPathName())
				load();
		}

		void setOption(const Identifier & identifier, var value) { state.setProperty(identifier, value, nullptr); }

		const var operator[](const Identifier & identifier) const { return state[identifier]; }

		void save() const
		{
			{
				InterProcessLock::ScopedLockType l(*lock);
				jcf::saveValueTreeToXml(file, state);
			}

			MessageManager::getInstance()->broadcastMessage(file.getFullPathName());
		}

		void load()
		{
			InterProcessLock::ScopedLockType l(*lock);

			auto newState = jcf::loadValueTreeFromXml(file);

			if (newState == ValueTree::invalid)
				newState = ValueTree("state");

			newState.addListener(this);

			std::set<Identifier> props;

			for (auto i = 0; i < newState.getNumProperties(); ++i)
				props.insert(newState.getPropertyName(i));

			for (auto i = 0; i < state.getNumProperties(); ++i)
				props.insert(state.getPropertyName(i));


			std::set<Identifier> propsChanged;

			for (auto prop : props)
				if (!state[prop].equals(newState[prop]))
					propsChanged.insert(prop);

			state = newState;

			for (auto prop: propsChanged)
				listeners.call(&Listener::optionsChanged, prop);
		}

		/**
		Sets an option if it doesn't already have a value in the configuration file.
		*/
		void setDefault(const Identifier & identifier, var defaultValue)
		{
			if (!state.hasProperty(identifier))
				setOption(identifier, defaultValue);
		}

		class Listener
		{
		public:
			virtual ~Listener() {}
			virtual void optionsChanged(const Identifier & identifierThatChanged) = 0;
		};

		void addListener(Listener* listener) { listeners.add(listener); }
		void removeListener(Listener* listener) { listeners.remove(listener); }

		/** You can attach data directly to the state tree, but the propertyChanged callback might not work properly */
		ValueTree state;
	private:
		void triggerTimer() { startTimer(1000); }

		void timerCallback() override
		{
			save();

			for (auto i : identifiersThatChanged) 
				listeners.call(&Listener::optionsChanged, i);

			identifiersThatChanged.clear();
			stopTimer();
		}

		void valueTreePropertyChanged(ValueTree&, const Identifier&identifier) override
		{
			triggerTimer();
			identifiersThatChanged.insert(identifier);
		}

		void valueTreeChildAdded(ValueTree&, ValueTree&) override { triggerTimer(); }
		void valueTreeChildRemoved(ValueTree&, ValueTree&, int) override { triggerTimer(); }
		void valueTreeChildOrderChanged(ValueTree&, int, int) override { triggerTimer(); }
		void valueTreeParentChanged(ValueTree&) override { triggerTimer(); }

		File file;
		std::set<Identifier> identifiersThatChanged;
		ScopedPointer<InterProcessLock> lock;
		ListenerList<Listener> listeners;
	};



#include "ui/jcf_font_awesome.h"
#include "utils/pitch.h"
#include "crypto/jcf_blowfish_extended.h"
#include "crypto/jcf_secure_credentials.h"

};


