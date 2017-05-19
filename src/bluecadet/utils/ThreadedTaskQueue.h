#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class ThreadedTaskQueue> ThreadedTaskQueueRef;
typedef std::shared_ptr<std::thread> ThreadRef;
typedef std::function<void(void)> TaskFn;

class ThreadedTaskQueue {

public:
	ThreadedTaskQueue();
	~ThreadedTaskQueue();

	//! Reconfigures the manager to spawn a new number of threads. Cancels and destroys all pending tasks. Running tasks will be completed. This method is thread-safe and potentially blocking.
	void setup(const int numThreads = 1);

	//! Cancels and destroys all pending tasks. Running tasks will be completed. This method is thread-safe and potentially blocking. Called automatically on app cleanup before exit.
	void destroy();

	//! Adds a task to the queue. Tasks are executed in first-in-first-out order. This method is thread-safe and potentially blocking.
	void addTask(TaskFn task);

	//! This may be a blocking method since it needs to sync with worker threads.
	size_t getNumPendingTasks();

protected:
	bool mIsCanceled;

	std::mutex mThreadMutex; // for thread management (starting, stopping, etc)
	std::mutex mTaskMutex; // for task processing and management
	std::condition_variable mTaskCondition;
	std::deque<TaskFn> mPendingTasks;
	std::vector<ThreadRef> mThreads;

	void processPendingTasks(); // Runs on worker thread

};

} // utils namespace
} // bluecadet namespace