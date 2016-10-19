#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "TimedTaskQueue.h"

using namespace ci;
using namespace ci::app;
using namespace std;

using namespace bluecadet::utils;

class TaskQueueSampleApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void draw() override;
	void createTasks();

	// Task queue is automatically started on construction and conifgured with infinite execution time
	TimedTaskQueue mTaskQueue;
};

void TaskQueueSampleApp::setup() {
	// Set the max execution time to stay below the duration of one frame to ensure a smooth frame rate
	mTaskQueue.setMaxExecutionTime(0.5 * 1.0 / (double)getFrameRate());
	createTasks();
}

void TaskQueueSampleApp::createTasks() {
	// Add a lot of tasks that will be executed in sequence but spread out across frames
	for (int i = 0; i < 100000; ++i) {
		mTaskQueue.add([=] {
			console() << "Running task " << to_string(i + 1) << " on frame " << getElapsedFrames() << endl;
		});
	}
}

void TaskQueueSampleApp::mouseDown(MouseEvent event) {
	createTasks();
}

void TaskQueueSampleApp::draw() {
	
	gl::clear(Color(0, 0, 0));

	// FPS should stay at 60 as tasks are executed
	gl::drawString("Click to add more tasks", vec2(0, 0), Color::white(), Font("Arial", 64));
	gl::drawString("Tasks remaining: " + to_string(mTaskQueue.getNumPendingTasks()), vec2(0, 64), Color::gray(0.5f), Font("Arial", 64));
	gl::drawString("FPS: " + to_string(getAverageFps()), vec2(0, 128), Color::gray(0.5f), Font("Arial", 64));
}

CINDER_APP(TaskQueueSampleApp, RendererGl)
