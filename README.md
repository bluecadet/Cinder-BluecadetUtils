# Cinder-BluecadetUtils

## [ImageManager](src/ImageManager.h)

The image manager loads and caches images from asset directories and makes them accessible via their filename.

## [ThreadManager](src/ThreadManager.h)

The thread manager can spawn internal worker threads and then runs tasks on those. This is helpful if you have expensive tasks like loading or compressing images. We used this in NASM to load images and data from the database or send analytics to Google.

Sample App: [samples/ThreadManagerSample/src/ThreadManagerSampleApp.cpp](samples/ThreadManagerSample/src/ThreadManagerSampleApp.cpp)

## [TimedTaskQueue](src/TimedTaskQueue.h)

The timed task queue runs on the current thread and automatically runs tasks that you give it on each frame for a certain amount of time. Once the time runs out it will resume running tasks on the next frame. This is helpful if you have to run a lot of tasks on the main thread but don't want to do it all in one frame to prevent stuttering. We used this in NASM for creating orbs, which all needed textures to be created on the main thread.

Sample App: [samples/TaskQueueSample/src/TaskQueueSampleApp.cpp](samples/TaskQueueSample/src/TaskQueueSampleApp.cpp)

Version 1.0.0
