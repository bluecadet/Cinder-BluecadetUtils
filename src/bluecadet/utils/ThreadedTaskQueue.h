#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class ThreadedTaskQueue> ThreadedTaskQueueRef;

class ThreadedTaskQueue {

public:
	typedef int TaskId;
	typedef std::function<void()> TaskFn;
	typedef std::function<void(int id)> TaskSuccessFn;
	typedef std::function<void(int id, bool canceled)> TaskFailureFn;

	ThreadedTaskQueue();
	~ThreadedTaskQueue();

	//! Reconfigures the manager to spawn a new number of threads. Cancels and destroys all pending tasks. Running tasks
	//! will be completed. This method is thread-safe and potentially blocking.
	void setup(const int numThreads = 1);

	//! Cancels and destroys all pending tasks. Running tasks will be completed. This method is thread-safe and
	//! potentially blocking. Called automatically on app cleanup before exit.
	void destroy();

	//! Adds a task to the queue. Tasks are executed in first-in-first-out order. This method is thread-safe and
	//! potentially blocking.
	TaskId addTask(TaskFn task, TaskSuccessFn successFn = nullptr, TaskFailureFn failureFn = nullptr);

	//! Attempts to cancel the pending task with taskId synchronously. Thread safe and will not throw explicit
	//! exceptions.
	bool cancelTask(TaskId taskId);

	//! This may be a blocking method since it needs to sync with worker threads.
	size_t getNumPendingTasks();

protected:
	struct Task {
		enum class State { Pending, Canceled, Completed };
		Task(TaskId id = -1, TaskFn fn = nullptr, TaskSuccessFn successFn = nullptr, TaskFailureFn failureFn = nullptr);
		TaskId id;
		TaskFn fn;
		TaskSuccessFn successFn = nullptr;
		TaskFailureFn failureFn = nullptr;
		State state				= State::Pending;
	};

	bool mIsCanceled = false;
	int mNumTasksCreated = 0;

	std::mutex mThreadMutex;  // for thread management (starting, stopping, etc)
	std::mutex mTaskMutex;	// for task processing and management
	std::condition_variable mTaskCondition;
	std::deque<Task> mPendingTasks;
	std::vector<std::thread> mThreads;

	void processPendingTasks();  // Runs on worker thread
};

}  // namespace utils
}  // namespace bluecadet