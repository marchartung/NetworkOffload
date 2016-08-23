# NetworkOffloader
A client/server interface for remote simulation and local visualization.  

Dependencies:

    * SDL2 (https://www.libsdl.org/download-2.0.php) 
      -> tested with version 2.04
    * SDL2_net (https://www.libsdl.org/projects/SDL_net/)
      -> tested with version 2.01
    * CMake 3.10 or higher (cmake.org)
      
Install:
  
  Create a build directory and use cmake, make and make install (tutorial: https://cmake.org/runningcmake/)
  
Example:
  
  An example directory is created with a small server/client application (SimpleSim) implementing NetOff. The sources are in the directory test of the source code. The SimpleSim application consists of two parts, the server and the client. The server is executed via `./SimpleSim 4444 whereas 4444` is a arbitrary chosen port. The client is started via `./SimpleSim 4444 localhost`.
This can also be done in one single console as `./SimpleSim 4444 & ./SimpleSim 4444 localhost`.
