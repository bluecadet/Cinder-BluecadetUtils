# Cinder-BluecadetUtils

## [Histogram](src/bluecadet/utils/Histogram.h)

The histogram is a lightweight implementation of a classic histogram with a fixed size. You can increment one value at a time or access all values directly. The class provides various helpers to compute the min/max etc.

Sample App: [samples/HistogramSample/src/HistogramSampleApp.cpp](samples/HistogramSample/src/HistogramSampleApp.cpp)

## [ImageManager](src/bluecadet/utils/ImageManager.h)

The image manager loads and caches images from asset directories and makes them accessible via their filename. This class is particularly useful when you have images that are reused often across all apps. Typically, the class is designed to load images at start up and store the textures on the GPU through the entire lifetime of your app.

## [AsyncImageLoader](src/bluecadet/utils/AsyncImageLoader.h)

The async image loader loads local and remote images while attempting to minimally block the main thread. Images are loaded and decoded on worker threads and are uploaded to the GPU in a linear queue across multiple frames. All images are cached and accessed by their path/url, but can be removed from the cache at any point. Pending image load operations can also be canceled at various stages of loading and decoding. This is helpful if your app needs to load many images on demand, that would be hard to cache in one big batch for the app's life time.

Sample App: [samples/AsyncImageLoadingSample/src/AsyncImageLoadingSampleApp.cpp](samples/AsyncImageLoadingSample/src/AsyncImageLoadingSampleApp.cpp)


## [ThreadedTaskQueue](src/bluecadet/utils/ThreadedTaskQueue.h)

The threaded qyeye can spawn internal worker threads and then run tasks on those. This is helpful if you have expensive tasks like loading or compressing images. We used this in NASM to load images and data from the database or send analytics to Google.

Sample App: [samples/ThreadedTaskQueueSample/src/ThreadedTaskQueueSampleApp.cpp](samples/ThreadedTaskQueueSample/src/ThreadedTaskQueueSampleApp.cpp)

## [TimedTaskQueue](src/bluecadet/utils/TimedTaskQueue.h)

The timed task queue runs on the current thread and automatically runs tasks that you give it on each frame for a certain amount of time. Once the time runs out it will resume running tasks on the next frame. This is helpful if you have to run a lot of tasks on the main thread but don't want to do it all in one frame to prevent stuttering. We used this in NASM for creating orbs, which all needed textures to be created on the main thread.

Sample App: [samples/TimedTaskQueueSample/src/TimedTaskQueueSampleApp.cpp](samples/TimedTaskQueueSample/src/TimedTaskQueueSampleApp.cpp)

Version 1.0.0
