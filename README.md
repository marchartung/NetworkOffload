# NetworkOffloader
A client/server interface for remote simulation and local visualization.

### Dependencies

Like most software projects NetworkOffloader uses some great ideas from other people:

    * SDL2 (https://www.libsdl.org/download-2.0.php) 
      -> tested with version 2.04
    * SDL2_net (https://www.libsdl.org/projects/SDL_net/)
      -> tested with version 2.01
    * CMake 3.1 or higher (cmake.org)

### Install

  Create a build directory and use cmake, make and make install (tutorial: https://cmake.org/runningcmake/), i.e, 
  ```
  cd NeworkOffloader/
  mkdir build && cd build
  cmake ../
  make
  ```

  On some machines, it might be necessary to give CMake a hint where to find the dependencies. How to do this, see the 
  CMakeLists file.

### Example

  An example directory is created with a small server/client application called *SimpleSim* implementing NetOff. The sources are in the directory test of the source code. The *SimpleSim* application consists of two parts, the server and the client. The server is executed via `./SimpleSim 4444` whereas 4444 is a arbitrary chosen port. The client is started via `./SimpleSim 4444 localhost`.
This can also be done in one single console as `./SimpleSim 4444 & ./SimpleSim 4444 localhost`.

The application *NetOffBenchmark* provides scalability tests. It is executed via `./NetOffBenchmark server bench & ./NetOffBenchmark client bench localhost`.
