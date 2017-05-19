#pragma once
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <queue>
#include <mutex>

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class TimedTaskQueue> TaskQueueRef;

class TimedTaskQueue {

public:
	typedef std::function<void(void)> TaskFn;

	TimedTaskQueue(const bool autoStart = true, double maxExecutionTime = -1.0);
	~TimedTaskQueue();

	//! Begins processing tasks with max execution time spread out over multiple frames.
	void start();

	//! Ends task processing; Does not clear any tasks.
	void stop();

	//! Clears all pending tasks. This method is thread-safe and blocking.
	void clear();

	//! Adds a task to the queue. Tasks are executed first-in-first-out. This method is thread-safe and blocking.
	void add(TaskFn task);

	//! Can be used to explicitly process all tasks regardless of how long they take. This method is thread-safe and blocking.
	void processAllTasks();

	//! Number of currently pending tasks. Includes the currently running task.
	int getNumPendingTasks() const { return (int)mPendingTasks.size(); };

	//! Will run tasks on each update call until max execution time is reached. Default is -1, which means infinite execution time.
	double getMaxExecutionTime() const { return mMaxExecutionTime; }
	void setMaxExecutionTime(const double value) { mMaxExecutionTime = value; }

protected:

	//! Called automatically when started, but can be called explicitly to process tasks separately. This method is thread-safe and blocking.
	void processTasks(double maxExecutionTime);

	ci::signals::Connection mMainLoopConnection;
	std::mutex			mTaskMutex;
	std::deque			<TaskFn>mPendingTasks;
	double				mMaxExecutionTime;

};

} // utils namespace
} // bluecadet namespace
