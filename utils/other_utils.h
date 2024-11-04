//
// Created by Jim Credland on 04/11/2024.
//

#pragma once
#include <juce_core/juce_core.h>

namespace jcf
{

#define JCF_LOG_CONSTRUCTOR                                                                                                                          \
    {                                                                                                                                                \
        std::stringstream s;                                                                                                                         \
        s << std::hex << int64 (this) << " constructed " << typeid (*this).name() << std::endl;                                                      \
        DBG (s.str());                                                                                                                               \
    }
#define JCF_LOG_DESTRUCTOR                                                                                                                           \
    {                                                                                                                                                \
        std::stringstream s;                                                                                                                         \
        s << std::hex << int64 (this) << " deleted " << typeid (*this).name() << std::endl;                                                          \
        DBG (s.str());                                                                                                                               \
    }
#define log_Ctor JCF_LOG_CONSTRUCTOR
#define log_Dtor JCF_LOG_DESTRUCTOR

#define JCF_ASSERT_THIS_IS_THE_MESSAGE_THREAD                                                                                                        \
    {                                                                                                                                                \
        jassert (juce::MessageManager::getInstance()->isThisTheMessageThread());                                                                     \
    }

/**
 * Return true if the file is a valid Windows filename.
 *
 * Note Windows also prohibits: Any other character that the target file system does not
 * allow.  We can't test for this here easily.
 */
bool isValidWindowsFilename (const juce::String& file);

bool isValidWindowsPathLength (const juce::File& file);
/**
 * Use this to own a task and block on destruction until it's complete.
 */
class LightweightThread : public juce::Thread
{
public:
    LightweightThread (std::function<void (Thread*)> func, int threadExitTime = 20000);

    ~LightweightThread();

    void run() override;
    std::function<void (Thread*)> func;
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
        const juce::SpinLock::ScopedLockType sl (holder.lock);

        if (--(holder.refCount) == 0)
            holder.sharedInstance.reset();
    }

    /** Returns the shared object, creating it if it doesn't already exist. */
    SharedObjectType& get() const
    {
        auto& holder = getSharedObjectHolder();
        const juce::SpinLock::ScopedLockType sl (holder.lock);

        if (holder.sharedInstance == nullptr)
            holder.sharedInstance.reset (new SharedObjectType());

        return *holder.sharedInstance;
    }

    /** Returns the number of SharedResourcePointers that are currently holding the shared object. */
    int getReferenceCount() const noexcept { return getSharedObjectHolder().refCount; }

private:
    struct SharedObjectHolder
    {
        juce::SpinLock lock;
        std::unique_ptr<SharedObjectType> sharedInstance;
        int refCount;
    };

    static SharedObjectHolder& getSharedObjectHolder() noexcept
    {
        static void* holder[(sizeof (SharedObjectHolder) + sizeof (void*) - 1) / sizeof (void*)] = { nullptr };
        return *reinterpret_cast<SharedObjectHolder*> (holder);
    }

    void initialise()
    {
        auto& holder = getSharedObjectHolder();
        const juce::SpinLock::ScopedLockType sl (holder.lock);

        ++holder.refCount;
    }

    // There's no need to assign to a SharedResourcePointer because every
    // instance of the class is exactly the same!
    DelayedSharedResourcePointer& operator= (const DelayedSharedResourcePointer&) = delete;

    JUCE_LEAK_DETECTOR (DelayedSharedResourcePointer)
};

class ApplicationActivtyMonitor : public juce::Timer, juce::MouseListener
{
public:
    /**
    timeoutSeconds is the length of time the application has to be in the
    background or not moving the mouse before we determine the user is
    not longer using the app.
     */
    ApplicationActivtyMonitor (int timeoutSeconds);

    ~ApplicationActivtyMonitor();

    void onApplicationBecomesActive (std::function<void()> fun);

    bool isApplicationRecentlyActive() const;

private:
    void mouseMove (const juce::MouseEvent&) override;

    void timerCallback() override;

    bool isActive{ true };
    int timeout;
    int counter{ 0 };
    std::function<void()> applicationNowActiveCallback;
};

template <typename ComponentType>
void addAndMakeVisibleComponent (juce::Component* parent, ComponentType& comp)
{
    parent->addAndMakeVisible (comp);
}

template <typename ComponentType, typename... Args>
void addAndMakeVisibleComponent (juce::Component* parent, ComponentType& comp, Args&... args)
{
    parent->addAndMakeVisible (comp);

    addAndMakeVisibleComponent (parent, args...);
}

template <typename ComponentType>
void addAndMakeVisibleComponent (juce::Component* parent, std::unique_ptr<ComponentType>& comp)
{
    parent->addAndMakeVisible (comp.get());
}

template <typename ComponentType, typename... Args>
void addAndMakeVisibleComponent (juce::Component* parent, std::unique_ptr<ComponentType>& comp, Args&... args)
{
    parent->addAndMakeVisible (comp.get());

    addAndMakeVisibleComponent (parent, args...);
}

template <typename DecimalType>
static juce::String toDecimalStringWithSignificantFigures (DecimalType number, int numberOfSignificantFigures)
{
    jassert (numberOfSignificantFigures > 0);

    if (number == 0)
    {
        if (numberOfSignificantFigures > 1)
        {
            juce::String result ("0.0");

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

juce::String bytesToFormattedString (juce::int64 bytes, int precision);

/**
 Loads a valuetree compatible XML file. Returna ValueTree::invalid if something goes wrong.
 */
juce::ValueTree loadValueTreeFromXml (const juce::File& file);

juce::Result saveValueTreeToXml (const juce::File& file, const juce::ValueTree& tree);

/** Saves a JUCE Array<> object in a binary ValueTree format */
template <class T>
class ArraySaver
{
public:
    ArraySaver (const juce::File& saveFile, const juce::Identifier& identifier) : identifier (identifier), saveFile (saveFile) {}

    virtual ~ArraySaver() = default;

    virtual juce::ValueTree serialize (const T& item) = 0;

    virtual T deserialize (const juce::ValueTree& t) = 0;

    juce::Result write (const juce::Array<T>& x)
    {
        juce::ValueTree t{ identifier };

        for (auto& e : x)
            t.addChild (serialize (e), -1, nullptr);

        if (saveFile.exists())
            if (! saveFile.deleteFile())
                return juce::Result::fail ("could not delete " + saveFile.getFullPathName());

        auto o = std::unique_ptr<juce::OutputStream> (saveFile.createOutputStream());

        if (o == nullptr)
            return juce::Result::fail ("could not write to " + saveFile.getFullPathName());

        t.writeToStream (*o);

        return juce::Result::ok();
    }

    bool removeItem (const T& item)
    {
        auto [r, a] = read();

        if (r.isNotEmpty())
            return false;

        a.removeAllInstancesOf (item);

        return write (a).wasOk();
    }

    /**
     * @returns an error string or the loaded array
     */
    std::pair<juce::String, juce::Array<T>> read()
    {
        auto i = std::unique_ptr<juce::InputStream> (saveFile.createInputStream());

        if (i == nullptr)
            return { "could not open " + saveFile.getFullPathName(), juce::Array<T>{} };

        auto t = juce::ValueTree::readFromStream (*i);

        if (t.getType() != identifier)
            return { "wrong format " + saveFile.getFullPathName(), juce::Array<T>{} };

        juce::Array<T> result;

        for (auto child : t)
            result.add (deserialize (child));

        return { {}, result };
    }

    juce::Identifier identifier;
    juce::File saveFile;
};

/** A very simple SVG or PNG rendering component */
class BasicImageComponent : public juce::Component
{
public:
    BasicImageComponent (const juce::File& file) { d = std::unique_ptr<juce::Drawable> (juce::Drawable::createFromImageFile (file)); }

    void replaceColour (const juce::Colour& original, const juce::Colour& newColour);

    void paint (juce::Graphics& g) override;

private:
    std::unique_ptr<juce::Drawable> d;
};

/**
 * When triggered calls a function no faster than the rate limit.  Calls it immediately if the
 * rate hasn't been exceeded.  Guarantees that the function is called at least once after each
 * trigger.  Can be triggered from any thread, the callback occurs on the message thread.
 */
class RateLimitedCallback : juce::Timer, juce::AsyncUpdater
{
public:
    RateLimitedCallback (std::function<void()> function, int rateLimitMilliSeconds);

    ~RateLimitedCallback();

    void trigger();

    void setRateLimit (int milliseconds);

private:
    void handleAsyncUpdate() override;

    void timerCallback() override;

    std::function<void()> function;
    int rateLimitMilliSeconds;
    bool updatePending{ false };
};

/**
 * Find the biggest rectangle of the given aspectRatio which will fit inside the outer.
 * \param outer will contain the result
 * \param widthByHeight the aspect ratio of the result calculated by width divided by height
 */
template <typename T>
juce::Rectangle<T> getRectangleWithAspectRatio (const juce::Rectangle<T>& outer, float widthByHeight)
{
    // true if height is the defining dimension e.g. the dimension which constrains the result
    const auto heightConstrains = static_cast<T> (outer.getHeight() * widthByHeight) < outer.getWidth();

    if (heightConstrains)
        return outer.withSizeKeepingCentre (static_cast<T> (outer.getHeight() * widthByHeight), outer.getHeight());

    return outer.withSizeKeepingCentre (outer.getWidth(), static_cast<T> (outer.getWidth() / widthByHeight));
}

/**
 * A function that vertically centres a vector of components maintaining their size and
 * spacing within a parent component.
 */
void centreComponentsVertically (std::vector<juce::Component*> components, const juce::Rectangle<int>& withInArea);

} // namespace jcf
