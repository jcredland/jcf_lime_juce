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
#include <iomanip>

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

#define JCF_LOG_CONSTRUCTOR {std::stringstream s; s << std::hex << int64(this) << " constructed " << typeid(*this).name() << std::endl; DBG(s.str());}
#define JCF_LOG_DESTRUCTOR {std::stringstream s; s << std::hex << int64(this) << " deleted " << typeid(*this).name() << std::endl; DBG(s.str());}
#define log_Ctor JCF_LOG_CONSTRUCTOR
#define log_Dtor JCF_LOG_DESTRUCTOR

#define JCF_ASSERT_THIS_IS_THE_MESSAGE_THREAD {jassert(MessageManager::getInstance()->isThisTheMessageThread());}
    using namespace juce;

    /** 
     * Use this to own a task and block on destruction until it's complete.
     */
    class LightweightThread : public Thread
    {
    public:
        LightweightThread(std::function<void(Thread *)> func, int threadExitTime = 20000) 
        : 
        Thread("lc lightweight thread"), func(std::move(func)), threadExitTime(threadExitTime)
        {
            startThread(5);
        }

        ~LightweightThread() { stopThread(threadExitTime); }

        void run () override { func(this); }
        std::function<void(Thread *)> func;
        int threadExitTime;
    };

    /**
     * Like SharedResourcePointer but delays construction of the object until
     * it's required.  We use this to optimise LookAndFeel creation in plugins
     * as font loading is slightly slow.
     */
    template <typename SharedObjectType>
    class DelayedSharedResourcePointer
    {
    public:
        DelayedSharedResourcePointer() { initialise(); }
        DelayedSharedResourcePointer (const DelayedSharedResourcePointer&) { initialise(); }

        /**
         * Destructor.  If no other DelayedSharedResourcePointer objects of this
         * type exist, this will also delete the shared object to which it refers.
         */
        ~DelayedSharedResourcePointer()
        {
            auto& holder = getSharedObjectHolder();
            const SpinLock::ScopedLockType sl (holder.lock);

            if (--(holder.refCount) == 0)
                holder.sharedInstance.reset();
        }

        /** Returns the shared object, creating it if it doesn't already exist. */
        SharedObjectType& get() const
        {
            auto& holder = getSharedObjectHolder();
            const SpinLock::ScopedLockType sl (holder.lock);

            if (holder.sharedInstance == nullptr)
                holder.sharedInstance.reset( new SharedObjectType() );

            return *holder.sharedInstance;
        }

        /** Returns the number of SharedResourcePointers that are currently holding the shared object. */
        int getReferenceCount() const noexcept { return getSharedObjectHolder().refCount; }

    private:
        struct SharedObjectHolder
        {
            SpinLock lock;
            std::unique_ptr<SharedObjectType> sharedInstance;
            int refCount;
        };

        static SharedObjectHolder& getSharedObjectHolder() noexcept
        {
            static void* holder[(sizeof (SharedObjectHolder) + sizeof(void*) - 1) / sizeof(void*)] = { nullptr };
            return *reinterpret_cast<SharedObjectHolder*> (holder);
        }

        void initialise()
        {
            auto& holder = getSharedObjectHolder();
            const SpinLock::ScopedLockType sl (holder.lock);

            ++holder.refCount;
        }

        // There's no need to assign to a SharedResourcePointer because every
        // instance of the class is exactly the same!
        DelayedSharedResourcePointer& operator= (const DelayedSharedResourcePointer&) = delete;

        JUCE_LEAK_DETECTOR (DelayedSharedResourcePointer)
    };

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
    
    template <typename ComponentType>
    void addAndMakeVisibleComponent(Component * parent, std::unique_ptr<ComponentType> & comp)
    {
        parent->addAndMakeVisible(comp.get());
    }
    
    template<typename ComponentType, typename... Args>
    void addAndMakeVisibleComponent(Component * parent, std::unique_ptr<ComponentType> & comp, Args&... args)
    {
        parent->addAndMakeVisible(comp.get());
        
        addAndMakeVisibleComponent(parent, args...);
    }
    
    template <typename DecimalType>
    static String toDecimalStringWithSignificantFigures (DecimalType number, int numberOfSignificantFigures)
    {
        jassert (numberOfSignificantFigures > 0);
        
        if (number == 0)
        {
            if (numberOfSignificantFigures > 1)
            {
                String result ("0.0");
                
                for (int i = 2; i < numberOfSignificantFigures; ++i)
                    result += "0";
                
                return result;
            }
            
            return "0";
        }
        
        auto numDigitsBeforePoint = (int) std::ceil (std::log10 (number < 0 ? -number : number));
        
        auto shift = numberOfSignificantFigures - numDigitsBeforePoint;
        auto factor = std::pow (10.0, shift);
        auto rounded = std::round (number * factor) / factor;
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision (std::max (shift, 0)) << rounded;
        return ss.str();
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

			return prefix + toDecimalStringWithSignificantFigures(b, precision) + units[unitIndex];
		}

		return String(0);
	}

    /**
     Loads a valuetree compatible XML file. Returna ValueTree::invalid if something goes wrong.
     */
    inline ValueTree loadValueTreeFromXml(const File & file)
    {
        std::unique_ptr<XmlElement> xml = std::unique_ptr<XmlElement>(XmlDocument(file.loadFileAsString()).getDocumentElement());

        if (xml == nullptr)
            return {};

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


	/** Saves a JUCE Array<> object in a binary ValueTree format */
	template<class T>
	class ArraySaver
	{
	public:
		ArraySaver(const File & saveFile, const Identifier & identifier)
				:
				identifier(identifier),
				saveFile(saveFile)
		{

		}
        
        virtual ~ArraySaver() {}

		virtual ValueTree serialize(const T & item) = 0;

		virtual T deserialize(const ValueTree & t) = 0;

		Result write(const Array<T> & x)
		{
			ValueTree t{ identifier };

			for (auto & e : x)
				t.addChild(serialize(e), -1, nullptr);

            saveFile.deleteFile();
            
			auto o = std::unique_ptr<OutputStream>(saveFile.createOutputStream());

			if (o == nullptr)
				return Result::fail("could not write to " + saveFile.getFullPathName());

			t.writeToStream(*o);

			return Result::ok();
		}

		bool removeItem(const T & item)
		{
			auto [r, a] = read();

			if (r.isNotEmpty())
				return false;

			a.removeAllInstancesOf(item);

			return write(a).wasOk();
		}

		/**
		 * @returns an error string or the loaded array
		 */
		std::pair<String, juce::Array<T>> read()
		{
			auto i = std::unique_ptr<InputStream>(saveFile.createInputStream());

			if (i == nullptr)
				return { "could not open " + saveFile.getFullPathName(), Array<T>{} };

			auto t = ValueTree::readFromStream(*i);

			if (t.getType() != identifier)
				return { "wrong format " + saveFile.getFullPathName(), Array<T>{} };

			juce::Array<T> result;

			for (auto child : t)
				result.add(deserialize(child));

			return { {}, result };
		}

		Identifier identifier;
		File saveFile;
	};

	/** A very simple SVG or PNG rendering component */
	class BasicImageComponent : public Component
	{
	public:
        BasicImageComponent(const File & file) { d = std::unique_ptr<Drawable>(Drawable::createFromImageFile(file)); }

		void replaceColour(const Colour & original, const Colour & newColour)
		{
            if (d != nullptr)
                d->replaceColour(original, newColour);
		}

		void paint(Graphics& g) override
		{
			if (d != nullptr)
				d->drawWithin(g, getLocalBounds().toFloat(), RectanglePlacement::centred, 1.0f);
		}

	private:
		std::unique_ptr<Drawable> d;
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
        
        ~RateLimitedCallback()
        {
            stopTimer();
        }

		void trigger()
		{
			triggerAsyncUpdate();
		}

		void setRateLimit(int milliseconds)
		{
			rateLimitMilliSeconds = milliseconds;
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
				//startTimer(rateLimitMilliSeconds);
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

    /**
    * Find the biggest rectangle of the given aspectRatio which will fit inside the outer.
    * \param outer will contain the result
    * \param widthByHeight the aspect ratio of the result calculated by width divided by height
    */
    template<typename T>
    juce::Rectangle<T> getRectangleWithAspectRatio (const juce::Rectangle<T>& outer, float widthByHeight)
    {
        // true if height is the defining dimension e.g. the dimension which constrains the result
        const auto heightConstrains = static_cast<T>(outer.getHeight ()* widthByHeight) < outer.getWidth ();

        if (heightConstrains)
            return outer.withSizeKeepingCentre (static_cast<T>(outer.getHeight () * widthByHeight), outer.getHeight ());

        return outer.withSizeKeepingCentre (outer.getWidth (), static_cast<T>(outer.getWidth () / widthByHeight));
    }


#include "ui/jcf_font_awesome.h"
#include "utils/pitch.h"
#include "crypto/jcf_blowfish_extended.h"
#include "crypto/jcf_secure_credentials.h"
#include "utils/lock_free_call_queue.h"
#include "utils/multi_async_updater.h"
#include "utils/app_options.h"

};


