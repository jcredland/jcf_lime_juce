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
#include <sstream>

#include <xmmintrin.h>

#ifdef _WIN32
#define EXPLORER_OR_FINDER "Explorer"
#else 
#define EXPLORER_OR_FINDER "Finder"
#endif

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


	class ApplicationActivtyMonitor
		:
		public Timer, MouseListener
	{
	public:
		/**
		timeoutSeconds is the length of time the application has to be in the
		background or not moving the mouse before we determine the user is
		not longer using the app.
		 */
		ApplicationActivtyMonitor(int timeoutSeconds) : timeout(timeoutSeconds)
		{
			startTimer(1000);
			Desktop::getInstance().addGlobalMouseListener(this);
		}

		~ApplicationActivtyMonitor()
		{
			Desktop::getInstance().removeGlobalMouseListener(this);
		}

		void onApplicationBecomesActive(std::function<void()> fun)
		{
			applicationNowActiveCallback = fun;
		}

		bool isApplicationRecentlyActive() const
		{
			return isActive;
		}

	private:
		void mouseMove(const MouseEvent&) override { counter = 0; }

		void timerCallback() override
		{
			if (counter == 0)
			{
				if (!isActive)
				{
					isActive = true;

					if (applicationNowActiveCallback)
						applicationNowActiveCallback();
				}

				counter++;
			}
			else
			{
				if (counter >= timeout)
					isActive = false;
				else
					counter++;
			}
		}

		bool isActive{ true };
		int timeout;
		int counter{ 0 };
		std::function<void()> applicationNowActiveCallback;
	};


	class ScopedNoDenormals
	{
	public:
		ScopedNoDenormals()
		{
			oldMXCSR = _mm_getcsr();
			int newMXCSR = oldMXCSR | 0x8040;
			_mm_setcsr(newMXCSR);
		};

		~ScopedNoDenormals() { _mm_setcsr(oldMXCSR); };

		int oldMXCSR;
	};

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

	inline String bytesToFormattedString(int64 bytes, int precision)
	{
		String prefix;
		String unit;

		std::vector<String> units = { "", "Kb", "Mb", "Gb", "Tb" };

		if (bytes < 0)
			prefix = "-";

        bytes = std::abs(bytes);

		if (bytes != 0)
		{
			auto unitIndex = int(std::floor(log(bytes) / log(1024))); // i.e. for 1024 we get 1, for 1024*1024 we get 2... 
			unitIndex = jlimit(0, int(units.size()) - 1, unitIndex);

			auto b = double(bytes) / std::pow(1024.0, unitIndex);

			return prefix + String(b, precision) + units[unitIndex];
		}

		return String(0);
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

	/** A very simple SVG or PNG rendering component */
	class BasicImageComponent : public Component
	{
	public:
		BasicImageComponent(const File & file) { d = Drawable::createFromImageFile(file); }

		void replaceColour(const Colour & original, const Colour & newColour)
		{
			d->replaceColour(original, newColour);
		}

		void paint(Graphics& g) override
		{
			if (d)
				d->drawWithin(g, getLocalBounds().toFloat(), RectanglePlacement::centred, 1.0f);
		}

	private:
		ScopedPointer<Drawable> d;
	};
	/**
	* When triggered calls a function no faster than the rate limit.  Calls it immediately if the
	* rate hasn't been exceeded.  Guarantees that the function is called at least once after each
	* trigger.  Can be triggered from any thread, the callback occurs on the message thread.
	*/
	class RateLimitedCallback : Timer, AsyncUpdater
	{
	public:
		RateLimitedCallback(std::function<void()> function, int rateLimitMilliSeconds) : function(function), rateLimitMilliSeconds(rateLimitMilliSeconds) {}

		void trigger()
		{
			triggerAsyncUpdate();
		}

	private:
		void handleAsyncUpdate() override
		{
			if (!isTimerRunning())
			{
				function();
				startTimer(rateLimitMilliSeconds);
			}
			else
			{
				startTimer(rateLimitMilliSeconds);
				updatePending = true;
			}
		}

		void timerCallback() override
		{
			stopTimer();

			if (updatePending)
			{
				function();
				updatePending = false;
			}
		}

		std::function<void()> function;
		int rateLimitMilliSeconds;
		bool updatePending{ false };
	};

	/** Note Windows also prohibits: Any other character that the target file system does not allow.  We can't test for this here easily. */
	bool isValidWindowsFilename(const String& file);

	bool isValidWindowsPathLength(const File& file);

#include "ui/jcf_font_awesome.h"
#include "utils/pitch.h"
#include "crypto/jcf_blowfish_extended.h"
#include "crypto/jcf_secure_credentials.h"
#include "utils/lock_free_call_queue.h"
#include "utils/multi_async_updater.h"
#include "utils/app_options.h"

};


