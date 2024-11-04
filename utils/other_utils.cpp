//
// Created by Jim Credland on 04/11/2024.
//

#include "other_utils.h"
namespace jcf
{
using namespace juce;
LightweightThread::LightweightThread (std::function<void (Thread*)> func, int threadExitTime)
    : Thread ("lc lightweight thread"), func (std::move (func)), threadExitTime (threadExitTime)
{
    startThread (Priority::normal);
}

LightweightThread::~LightweightThread()
{
    stopThread (threadExitTime);
}

void LightweightThread::run()
{
    func (this);
}

ApplicationActivtyMonitor::ApplicationActivtyMonitor (int timeoutSeconds) : timeout (timeoutSeconds)
{
    startTimer (1000);
    Desktop::getInstance().addGlobalMouseListener (this);
}

ApplicationActivtyMonitor::~ApplicationActivtyMonitor()
{
    Desktop::getInstance().removeGlobalMouseListener (this);
}

void ApplicationActivtyMonitor::onApplicationBecomesActive (std::function<void()> fun)
{
    applicationNowActiveCallback = fun;
}

bool ApplicationActivtyMonitor::isApplicationRecentlyActive() const
{
    return isActive;
}

void ApplicationActivtyMonitor::mouseMove (const MouseEvent& mouse_event)
{
    counter = 0;
}

void ApplicationActivtyMonitor::timerCallback()
{
    if (counter == 0)
    {
        if (! isActive)
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

String bytesToFormattedString (int64 bytes, int precision)
{
    String prefix;
    String unit;

    std::vector<String> units = { "", "Kb", "Mb", "Gb", "Tb" };

    if (bytes < 0)
        prefix = "-";

    bytes = std::abs (bytes);

    if (bytes != 0)
    {
        auto unitIndex = int (std::floor (log (bytes) / log (1024))); // i.e. for 1024 we get 1, for 1024*1024 we get 2...
        unitIndex = jlimit (0, int (units.size()) - 1, unitIndex);

        auto b = double (bytes) / std::pow (1024.0, unitIndex);

        return prefix + toDecimalStringWithSignificantFigures (b, precision) + units[unitIndex];
    }

    return String (0);
}

juce::ValueTree loadValueTreeFromXml (const File& file)
{
    std::unique_ptr<XmlElement> xml = std::unique_ptr<XmlElement> (XmlDocument (file.loadFileAsString()).getDocumentElement());

    if (xml == nullptr)
        return {};

    auto tree = ValueTree::fromXml (*xml);
    return tree;
}

Result saveValueTreeToXml (const File& file, const ValueTree& tree)
{
    auto string = tree.toXmlString();
    auto result = file.replaceWithText (string);

    if (! result)
        return Result::fail ("Save failed to " + file.getFullPathName());

    return Result::ok();
}

void BasicImageComponent::replaceColour (const Colour& original, const Colour& newColour)
{
    if (d != nullptr)
        d->replaceColour (original, newColour);
}

void BasicImageComponent::paint (Graphics& g)
{
    if (d != nullptr)
        d->drawWithin (g, getLocalBounds().toFloat(), RectanglePlacement::centred, 1.0f);
}

RateLimitedCallback::RateLimitedCallback (std::function<void()> function, int rateLimitMilliSeconds)
    : function (function), rateLimitMilliSeconds (rateLimitMilliSeconds)
{
}

RateLimitedCallback::~RateLimitedCallback()
{
    stopTimer();
    JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED; // async updater
}

void RateLimitedCallback::trigger()
{
    triggerAsyncUpdate();
}

void RateLimitedCallback::setRateLimit (int milliseconds)
{
    rateLimitMilliSeconds = milliseconds;
}

void RateLimitedCallback::handleAsyncUpdate()
{
    if (! isTimerRunning())
    {
        function();
        startTimer (rateLimitMilliSeconds);
    }
    else
    {
        // startTimer(rateLimitMilliSeconds);
        updatePending = true;
    }
}

void RateLimitedCallback::timerCallback()
{
    stopTimer();

    if (updatePending)
    {
        function();
        updatePending = false;
    }
}

void centreComponentsVertically (std::vector<Component*> components, const Rectangle<int>& withInArea)
{
    if (components.empty())
        return;

    auto minY = 0;
    auto maxY = 0;

    for (auto c : components)
    {
        minY = std::min (minY, c->getY());
        maxY = std::max (maxY, c->getBottom());
    }

    auto totalHeight = maxY - minY;

    if (totalHeight == 0)
        return;

    if (totalHeight > withInArea.getHeight())
    {
        jassertfalse;
        return;
    }

    auto yOffset = withInArea.getY() + (withInArea.getHeight() - totalHeight) / 2;
    auto originalY = components[0]->getY();

    for (auto c : components)
        c->setTopLeftPosition (c->getX(), c->getY() - originalY + yOffset);
}

bool isValidWindowsFilename (const String& file)
{
    if (file.containsAnyOf ("<>:\"/\\|?*"))
        return false;

    for (auto t (file.getCharPointer()); ! t.isEmpty();)
        if (t.getAndAdvance() < ' ')
            return false;

    auto ucFile = file.toUpperCase();

    if (String ("CPANL").containsChar (ucFile[0]))
    {
        const char* reservedNames[] = { "CON",  "PRN",  "AUX",  "NUL",  "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7",
                                        "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9" };

        for (auto& r : reservedNames)
            if (ucFile == r)
                return false;
    }

    if (file.endsWithChar ('.'))
        return false;

    return true;
}

bool isValidWindowsPathLength (const File& file)
{
    auto p = file.getFullPathName();

    if (p.length() > 260)
        return false;

    return true;
}

} // namespace jcf
