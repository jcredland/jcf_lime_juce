#pragma once

class MultiAsyncUpdater
	:
	AsyncUpdater
{
public:
	MultiAsyncUpdater()
	{}

	// pass by value is more efficient where the std::function will be created in place
	// from a lambda.  See http://stackoverflow.com/questions/18365532/should-i-pass-an-stdfunction-by-const-reference
	void callOnMessageThread(std::function<void()> callback)
	{
		ScopedLock l(lock);
		queue.push_back(std::move(callback));
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

	CriticalSection lock;
	std::vector<std::function<void()>> queue;
};

