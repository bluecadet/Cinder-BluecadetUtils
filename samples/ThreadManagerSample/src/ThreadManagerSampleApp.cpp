#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/Utilities.h"

#include "ThreadManager.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::utils;

class ThreadManagerSampleApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void draw() override;
	void createTasks();

	ThreadManager mThreadManager;
};

void ThreadManagerSampleApp::setup() {
	// Spawn multiple threads that can each execute one task at a time w/o blocking the main thread
	mThreadManager.setup(4);
	createTasks();
}

void ThreadManagerSampleApp::createTasks() {
	for (int i = 0; i < 100000; ++i) {
		mThreadManager.addTask([=] {
			// Artificially long task that will run on a worker thread w/o blocking the main thread
			int sum = 0;
			for (int j = 0; j < 100000; ++j) sum = j % 100;
			cout << "Finished task " << to_string(i + 1) << " (computed sum: " << sum << ") on frame " << getElapsedFrames() << endl;
		});
	}
}

void ThreadManagerSampleApp::mouseDown(MouseEvent event) {
	createTasks();
}

void ThreadManagerSampleApp::draw() {

	gl::clear(Color(0, 0, 0));

	// FPS should stay at 60 as tasks are executed
	gl::drawString("Click to add more tasks", vec2(0, 0), Color::white(), Font("Arial", 64));
	gl::drawString("Tasks remaining: " + to_string(mThreadManager.getNumPendingTasks()), vec2(0, 64), Color::gray(0.5f), Font("Arial", 64));
	gl::drawString("FPS: " + to_string(getAverageFps()), vec2(0, 128), Color::gray(0.5f), Font("Arial", 64));
}

CINDER_APP(ThreadManagerSampleApp, RendererGl)
