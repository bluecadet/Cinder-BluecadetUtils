#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ConcurrentCircularBuffer.h"

#include "ThreadedTaskQueue.h"
#include "TimedTaskQueue.h"

namespace bluecadet {
namespace utils {

typedef std::shared_ptr<class AsyncGlQueue> AsyncGlQueueRef;
	
class AsyncGlQueue {
	
public:

	//! Optional shared instance. This class can still be independently instantiated.
	static AsyncGlQueueRef getInstance() {
		static AsyncGlQueueRef instance = nullptr;
		if (!instance) {
			instance = std::make_shared<AsyncGlQueue>();
		}
		return instance;
	};

	// Callback type for tasks
	typedef std::function<void()> Task;
	typedef std::function<void(bool completed, bool canceled)> Callback;

	struct TaskInfo {
		TaskInfo(Task task = nullptr, Callback callback = nullptr) : task(task), callback(callback) {};
		Task task = nullptr;			//! This will be called on a GL sub-thread
		Callback callback = nullptr;	//! This will be called on the main thread
		bool isCompleted = false;
		bool isCanceled = false;
	};
	
	//! numThreads: Threads used for executing tasks.
	//! queueSize: The max number of tasks that can be processed per frame. The min size is set to numThreads * 2
	AsyncGlQueue(const unsigned int numThreads = 1, const unsigned int queueSize = 1024);
	virtual ~AsyncGlQueue();
	
	void run(Task task, Callback optCallback = nullptr);
	void cancelAll(); // cancels any pending tasks

	void setNumThreads(const unsigned int value) { if (mNumThreads != value) { mNumThreads = value; setup(); } }
	unsigned int getNumThreads() const { return mNumThreads; }

protected:
	void setup();
	void threadLoop(ci::gl::ContextRef context);
	void processTasks();

	static void initializeLoader(); // makes sure that Cinder's internal static factories are initialized once on the main thread
	static bool sIsInitialized; // need to initialize Cinder image factory on main thread 
	static std::mutex mInitializationMutex;

	unsigned int mNumThreads = -1;

	std::set<ci::gl::ContextRef> mBackgroundContexts;

	std::deque<TaskInfo> mTodos;
	ci::ConcurrentCircularBuffer<TaskInfo> mProcessing;
	ci::ConcurrentCircularBuffer<TaskInfo> mCompleted;

	std::set<std::shared_ptr<std::thread>> mThreads;
	std::atomic<bool> mThreadsAreAlive = true;
	ci::signals::ConnectionList mSignalConnections;
};

}
}
