
#include "AsyncGlQueue.h"

#include "cinder/Log.h"
#include "cinder/Filesystem.h"
#include "cinder/imageIo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

namespace bluecadet {
namespace utils {
	
	// Static properties
	bool AsyncGlQueue::sIsInitialized = false;
	std::mutex AsyncGlQueue::mInitializationMutex;

	AsyncGlQueue::AsyncGlQueue(const unsigned int numThreads, const unsigned int queueSize) :
		mNumThreads(numThreads),
		mProcessing(max(mNumThreads * 2, queueSize)),
		mCompleted(max(mNumThreads * 2, queueSize))
	{
	}
	
	AsyncGlQueue::~AsyncGlQueue() {
		mProcessing.cancel();
		mCompleted.cancel();
		mThreadsAreAlive = false;
		for (auto thread : mThreads) {
			thread->join();
		}
	}

	void AsyncGlQueue::run(Task task, Callback optCallback) {
		setup();
		mTodos.push_back(TaskInfo(task, optCallback));
	}

	void AsyncGlQueue::cancelAll() {
		TaskInfo info;
		while (mCompleted.tryPopBack(&info)) {}
	}

	void AsyncGlQueue::threadLoop(ci::gl::ContextRef context) {
		ci::ThreadSetup threadSetup;

		context->makeCurrent();
		initializeLoader();

		while (mThreadsAreAlive) {
			try {

				TaskInfo info;

				mProcessing.popBack(&info);

				if (!mThreadsAreAlive) {
					return;
				}

				if (!info.task) {
					CI_LOG_E("Could not fetch new task");
					continue;
				}

				// execute task
				info.task();

				// create fence after all gpu commands
				auto fence = gl::Sync::create();

				// waits until fence has been executed
				fence->clientWaitSync();

				info.isCompleted = true;

				if (!mThreadsAreAlive) {
					return;
				}

				// queue for completion
				mCompleted.pushFront(info);

			}
			catch (ci::Exception e) {
				CI_LOG_EXCEPTION("Could not run task", e);
			}
		}
	}

	void AsyncGlQueue::setup() {
		// only set up if # threads has changed
		if (mNumThreads == mThreads.size()) {
			return;
		}

		// initialize from primary thread
		App::get()->dispatchSync([=] {
			
			// Save current VAO to prevent crashes from calling setup multiple times from params
			gl::Context::getCurrent()->pushVao();

			try {
				mSignalConnections.clear();

				mThreadsAreAlive = false;
				//mTodoLock.notify_all();
				for (auto thread : mThreads) {
					thread->join();
				}
				mThreads.clear();
				mBackgroundContexts.clear();
				mThreadsAreAlive = true;

				for (unsigned int i = 0; i < mNumThreads; ++i) {
					auto context = gl::Context::create(gl::Context::getCurrent());
					mBackgroundContexts.insert(context);

					auto thread = make_shared<std::thread>(bind(&AsyncGlQueue::threadLoop, this, context));
					mThreads.insert(thread);
				}

				mSignalConnections += App::get()->getSignalUpdate().connect(bind(&AsyncGlQueue::processTasks, this));

			} catch (std::exception & e) {
				CI_LOG_EXCEPTION("Error in setup", e);
			}

			// Restore previous VAO to prevent crashes from calling setup multiple times from params
			gl::Context::getCurrent()->popVao();
		});
	}

	void AsyncGlQueue::processTasks() {
		// todos
		while (!mTodos.empty() && mProcessing.tryPushFront(mTodos.back())) {
			mTodos.pop_back();
		}

		// completed
		TaskInfo info;
		while (mCompleted.tryPopBack(&info)) {
			if (info.callback) {
				info.callback(info.isCompleted, info.isCanceled);
			}
		}
	}

	void AsyncGlQueue::initializeLoader() {
		lock_guard<mutex> lock(mInitializationMutex);

		if (sIsInitialized) {
			return;
		}

		App::get()->dispatchSync([=] {
			try {
				// load empty image to initialize Cinder's internal load factory on the main thread
				CI_LOG_D("Initializing Cinder loader");
				ci::loadImage("");
			} catch (Exception e) {
			}
			sIsInitialized = true;
		});
	}
	
}
}
