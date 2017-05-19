#include "ThreadedTaskQueue.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ThreadedTaskQueue::ThreadedTaskQueue() {
	AppBase::get()->getSignalCleanup().connect(bind(&ThreadedTaskQueue::destroy, this));
}

ThreadedTaskQueue::~ThreadedTaskQueue() {
	destroy();
}

void ThreadedTaskQueue::setup(const int numThreads) {
	destroy();

	lock_guard<mutex> lock(mThreadMutex);
	mIsCanceled = false;

	for (int i = 0; i < numThreads; ++i) {
		try {
			ThreadRef thread = ThreadRef(new std::thread(bind(&ThreadedTaskQueue::processPendingTasks, this)));
			mThreads.push_back(thread);

		} catch (Exception e) {
			cout << "ThreadedTaskQueue: Could not start worker thread: " << e.what() << endl;
		}
	}

	mTaskCondition.notify_all();

	cout << "ThreadedTaskQueue: Started " << to_string(mThreads.size()) << " worked threads" << endl;
}

void ThreadedTaskQueue::destroy() {
	lock_guard<mutex> lock(mThreadMutex);

	mIsCanceled = true;
	mTaskCondition.notify_all();

	for (auto thread : mThreads) {
		try {
			if (thread && thread->joinable()) {
				thread->join();
			}
		} catch (Exception e) {
			cout << "ThreadedTaskQueue: Could not stop worker thread: " << e.what() << endl;
		}
	}

	mThreads.clear();
}

void ThreadedTaskQueue::addTask(TaskFn task) {
	try {
		unique_lock<mutex> lock(mTaskMutex);
		mPendingTasks.push_back(task);
		mTaskCondition.notify_one();

	} catch (Exception e) {
		cout << "ThreadedTaskQueue: Could not add task: " << e.what() << endl;
	}
}

size_t ThreadedTaskQueue::getNumPendingTasks() {
	unique_lock<mutex> lock(mTaskMutex);
	return mPendingTasks.size();
}

void ThreadedTaskQueue::processPendingTasks() {
	while (true) {
		TaskFn task = nullptr;

		try {
			// grab a task from the queue
			unique_lock<mutex> lock(mTaskMutex);

			while (!mIsCanceled && mPendingTasks.empty()) {
				mTaskCondition.wait(lock); // wait
			}

			if (mIsCanceled) {
				return; // cancel
			}

			task = mPendingTasks.front();
			mPendingTasks.pop_front();

		} catch (Exception e) {
			cout << "ThreadedTaskQueue: Error while processing tasks: " << e.what() << endl;
		}

		if (task) {
			try {
				// run task
				task();
			} catch (Exception e) {
				cout << "ThreadedTaskQueue: Error while executing task: " << e.what() << endl;
			}
		}
	}
}

} // utils namespace
} // bluecadet namespace
