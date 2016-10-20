# Cinder-BluecadetUtils

## [Histogram](src/Histogram.h)

The histogram is a lightweight implementation of a classic histogram with a fixed size. You can increment one value at a time or access all values directly. The class provides various helpers to compute the min/max etc.

Sample App: [samples/HistogramSample/src/HistogramSampleApp.cpp](samples/HistogramSample/src/HistogramSampleApp.cpp)

## [ImageManager](src/ImageManager.h)

The image manager loads and caches images from asset directories and makes them accessible via their filename.

## [ThreadedTaskQueue](src/ThreadedTaskQueue.h)

The threaded qyeye can spawn internal worker threads and then run tasks on those. This is helpful if you have expensive tasks like loading or compressing images. We used this in NASM to load images and data from the database or send analytics to Google.

Sample App: [samples/ThreadedTaskQueueSample/src/ThreadedTaskQueueSampleApp.cpp](samples/ThreadedTaskQueueSample/src/ThreadedTaskQueueSampleApp.cpp)

## [TimedTaskQueue](src/TimedTaskQueue.h)

The timed task queue runs on the current thread and automatically runs tasks that you give it on each frame for a certain amount of time. Once the time runs out it will resume running tasks on the next frame. This is helpful if you have to run a lot of tasks on the main thread but don't want to do it all in one frame to prevent stuttering. We used this in NASM for creating orbs, which all needed textures to be created on the main thread.

Sample App: [samples/TimedTaskQueueSample/src/TimedTaskQueueSampleApp.cpp](samples/TimedTaskQueueSample/src/TimedTaskQueueSampleApp.cpp)

Version 1.0.0
