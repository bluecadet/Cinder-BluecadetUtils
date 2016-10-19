#include "ThreadManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {

ThreadManager::ThreadManager() {
	AppBase::get()->getSignalCleanup().connect(bind(&ThreadManager::destroy, this));
}

ThreadManager::~ThreadManager() {
	destroy();
}

void ThreadManager::setup(const int numThreads) {
	destroy();

	lock_guard<mutex> lock(mThreadMutex);
	mIsCanceled = false;

	for (int i = 0; i < numThreads; ++i) {
		try {
			ThreadRef thread = ThreadRef(new std::thread(bind(&ThreadManager::processPendingTasks, this)));
			mThreads.push_back(thread);

		} catch (Exception e) {
			cout << "ThreadManager: Could not start worker thread: " << e.what() << endl;
		}
	}

	mTaskCondition.notify_all();

	cout << "ThreadManager: Started " << to_string(mThreads.size()) << " worked threads" << endl;
}

void ThreadManager::destroy() {
	lock_guard<mutex> lock(mThreadMutex);

	mIsCanceled = true;
	mTaskCondition.notify_all();

	for (auto thread : mThreads) {
		try {
			if (thread && thread->joinable()) {
				thread->join();
			}
		} catch (Exception e) {
			cout << "ThreadManager: Could not stop worker thread: " << e.what() << endl;
		}
	}

	mThreads.clear();
}

void ThreadManager::addTask(TaskFn task) {
	try {
		unique_lock<mutex> lock(mTaskMutex);
		mPendingTasks.push_back(task);
		mTaskCondition.notify_one();

	} catch (Exception e) {
		cout << "ThreadManager: Could not add task: " << e.what() << endl;
	}
}

size_t ThreadManager::getNumPendingTasks() {
	unique_lock<mutex> lock(mTaskMutex);
	return mPendingTasks.size();
}

void ThreadManager::processPendingTasks() {
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
			cout << "ThreadManager: Error while processing tasks: " << e.what() << endl;
		}

		if (task) {
			try {
				// run task
				task();
			} catch (Exception e) {
				cout << "ThreadManager: Error while executing task: " << e.what() << endl;
			}
		}
	}
}

} // utils namespace
} // bluecadet namespace
