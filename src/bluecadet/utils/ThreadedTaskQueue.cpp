#include "ThreadedTaskQueue.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ThreadedTaskQueue::ThreadedTaskQueue() {
	AppBase::get()->getSignalCleanup().connect(bind(&ThreadedTaskQueue::destroy, this));
}

ThreadedTaskQueue::~ThreadedTaskQueue() { destroy(); }

void ThreadedTaskQueue::setup(const int numThreads) {
	destroy();

	lock_guard<mutex> lock(mThreadMutex);
	mIsCanceled = false;

	for (int i = 0; i < numThreads; ++i) {
		try {
			mThreads.push_back(std::thread(bind(&ThreadedTaskQueue::processPendingTasks, this)));

		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not start worker thread.", e);
		}
	}

	mTaskCondition.notify_all();

	CI_LOG_I("Started " << to_string(mThreads.size()) << " worker threads");
}

void ThreadedTaskQueue::destroy() {
	lock_guard<mutex> lock(mThreadMutex);

	mIsCanceled = true;
	mTaskCondition.notify_all();

	for (auto & thread : mThreads) {
		try {
			if (thread.joinable()) {
				thread.join();
			}
		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not stop worker thread.", e);
		}
	}

	mThreads.clear();
}

ThreadedTaskQueue::TaskId ThreadedTaskQueue::addTask(TaskFn fn, TaskSuccessFn successFn, TaskFailureFn failureFn) {
	try {
		unique_lock<mutex> lock(mTaskMutex);
		mNumTasksCreated = (mNumTasksCreated + 1) % INT_MAX;
		TaskId id		 = mNumTasksCreated;
		mPendingTasks.push_back(Task(id, fn, successFn, failureFn));
		mTaskCondition.notify_one();
		return id;

	} catch (Exception e) {
		CI_LOG_EXCEPTION("Could not add task.", e);
		return -1;
	}
}

bool ThreadedTaskQueue::cancelTask(TaskId taskId) {
	unique_lock<mutex> lock(mTaskMutex);
	for (auto it = mPendingTasks.begin(); it != mPendingTasks.end(); ++it) {
		if (it->id == taskId) {
			it->state = Task::State::Canceled;
			if (it->failureFn) {
				it->failureFn(it->id, true);
			}
			mPendingTasks.erase(it);
			return true;
		}
	}
	return false;
}

size_t ThreadedTaskQueue::getNumPendingTasks() {
	unique_lock<mutex> lock(mTaskMutex);
	return mPendingTasks.size();
}

void ThreadedTaskQueue::processPendingTasks() {
	while (true) {
		Task task;

		try {
			// grab a task from the queue
			unique_lock<mutex> lock(mTaskMutex);

			while (!mIsCanceled && mPendingTasks.empty()) {
				mTaskCondition.wait(lock);  // wait
			}

			if (mIsCanceled) {
				return;  // cancel
			}

			task = std::move(mPendingTasks.front());
			mPendingTasks.pop_front();

		} catch (Exception e) {
			CI_LOG_EXCEPTION("Could not fetch next task.", e);
		}

		if (task.id != -1 && task.fn) {
			try {
				// run task
				task.fn();
				task.state = Task::State::Completed;
				if (task.successFn) {
					task.successFn(task.id);
				}
			} catch (Exception e) {
				CI_LOG_EXCEPTION("Could not execute task.", e);
			}
		}
	}
}

ThreadedTaskQueue::Task::Task(TaskId id, TaskFn fn, TaskSuccessFn successFn, TaskFailureFn failureFn)
	: id(id), fn(std::move(fn)), successFn(std::move(successFn)), failureFn(std::move(failureFn)) {}

}  // namespace utils
}  // namespace bluecadet
