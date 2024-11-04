#pragma once
#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace jcf
{
class MultiAsyncUpdater : juce::AsyncUpdater
{
public:
    MultiAsyncUpdater() = default;

    ~MultiAsyncUpdater()
    {
        JUCE_ASSERT_MESSAGE_MANAGER_IS_LOCKED; // async updater
    }

    // pass by value is more efficient where the std::function will be created in place
	// from a lambda.  See http://stackoverflow.com/questions/18365532/should-i-pass-an-stdfunction-by-const-reference
	void callOnMessageThread(std::function<void()> callback)
	{
        juce::ScopedLock l (lock);
        queue.push_back (std::move (callback));
        triggerAsyncUpdate();
    }

private:
	void handleAsyncUpdate() override
	{
		lock.enter();
		auto queueCopy = queue;
		queue.clear();
		lock.exit();

		for (auto & func : queueCopy)
            func();
    }

    juce::CriticalSection lock;
    std::vector<std::function<void()>> queue;
};

} // namespace jcf