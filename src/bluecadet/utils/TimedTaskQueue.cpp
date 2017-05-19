#include "TimedTaskQueue.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

TimedTaskQueue::TimedTaskQueue(const bool autoStart, const double maxExecutionTime) {
	mMaxExecutionTime = maxExecutionTime;
	if (autoStart) {
		start();
	}
}

TimedTaskQueue::~TimedTaskQueue() {
	mMainLoopConnection.disconnect();
}

void TimedTaskQueue::add(TaskFn task) {
	std::lock_guard<std::mutex> lock(mTaskMutex);
	mPendingTasks.push_back(task);
}

void TimedTaskQueue::start() {
	mMainLoopConnection.disconnect();
	mMainLoopConnection = AppBase::get()->getSignalUpdate().connect(bind(&TimedTaskQueue::processTasks, this, mMaxExecutionTime));
}

void TimedTaskQueue::stop() {
	mMainLoopConnection.disconnect();
}

void TimedTaskQueue::clear() {
	std::lock_guard<std::mutex> lock(mTaskMutex);
	mPendingTasks.clear();
}

void TimedTaskQueue::processAllTasks() {
	std::lock_guard<std::mutex> lock(mTaskMutex);

	while (!mPendingTasks.empty()) {
		TaskFn task = mPendingTasks.front();
		task();
		mPendingTasks.pop_front();
	}
}

void TimedTaskQueue::processTasks(double maxExecutionTime) {
	std::lock_guard<std::mutex> lock(mTaskMutex);

	double startTime = getElapsedSeconds();

	while (!mPendingTasks.empty()) {
		TaskFn task = mPendingTasks.front();
		mPendingTasks.pop_front();
		task();

		if (mMaxExecutionTime >= 0.0 && getElapsedSeconds() - startTime >= mMaxExecutionTime) {
			break;
		}
	}
}


} // utils namespace
} // bluecadet namespace

