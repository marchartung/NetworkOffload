# CMakefile to configure and build NetworkOffloader [https://github.com/marchartung/NetworkOffloader]
#
# Authors: Marc Hartung, Martin Flehmig
#
# On some machines, it might be necessary to give CMake a hint where to find the dependencies:
# -DSDL2_LIBRARY=/PATH/TO/SDL2/lib/libSDL2.so
# -DSDL2MAIN_LIBRARY=/PATH/TO/SDL2/lib/libSDL2.so ../
# -DSDL2_NET_LIBRARY=/PATH/TO/SDL2_net/lib/libSDL2_net.so
# -DSDL2_NET_INCLUDE_DIR=/PATH/TO/SDL2_net/include/SDL2/
#
# Attention: The library paths have to point to the library that should be used, not only the library directory! This 
#            is due to the FindSDL2 and FindSDL2_net scripts we use.
#

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project(NetworkOffloader)

set(CMAKE_VERBOSE_MAKEFILE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
message(STATUS "Using Cmake version ${CMAKE_VERSION}")

# ---------------------------
# Find all dependencies
# ---------------------------
# SDL2
find_package(SDL2)
if(NOT(SDL2_FOUND))
  message(FATAL_ERROR "SDL2 required")
else(NOT(SDL2_FOUND))
  message(STATUS "SDL2_INCLUDE_DIRS = ${SDL2_INCLUDE_DIRS}")
  message(STATUS "SDL2_LIBRARIES = ${SDL2_LIBRARIES}")
  message(STATUS "SDL2_VERSION_STRING = ${SDL2_VERSION_STRING}")
endif(NOT(SDL2_FOUND))

# SDL2_Net
find_package(SDL2_net)
if(NOT(SDL2_NET_FOUND))
  message(FATAL_ERROR "SDL2_net required")
else(NOT(SDL2_NET_FOUND))
  message(STATUS "SDL2_NET_LIBRARIES = ${SDL2_NET_LIBRARIES}")
  message(STATUS "SDL2_NET_INCLUDE_DIRS = ${SDL2_NET_INCLUDE_DIRS}")
endif(NOT(SDL2_NET_FOUND))


# ---------------------------
# Compiler Flags
# ---------------------------
# Check for C++11 Support
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
IF(COMPILER_SUPPORTS_CXX11)
  ADD_COMPILE_OPTIONS(-std=c++11 -Wall)
ELSEIF(COMPILER_SUPPORTS_CXX0X)
  ADD_COMPILE_OPTIONS(-std=c++0x -Wall)
ELSE(COMPILER_SUPPORTS_CXX11)
  MESSAGE(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF(COMPILER_SUPPORTS_CXX11)
# Compiler Flags Used In Debug Mode
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_COMPILE_OPTIONS(-O0 -Weffc++)
  #-fno-elide-constructors -pedantic-errors -ansi -Winit-self -Wold-style-cast -Woverloaded-virtual -Wuninitialized -Wmissing-declarations -Winit-self)

  # Check for Compiler Warnings
  CHECK_CXX_COMPILER_FLAG("-Wextra -Wunused" COMPILER_SUPPORTS_WEXTRA)
  IF(COMPILER_SUPPORTS_WEXTRA)
    ADD_COMPILE_OPTIONS(-Wextra -Wunused)
  ENDIF(COMPILER_SUPPORTS_WEXTRA)
ENDIF(CMAKE_BUILD_TYPE MATCHES Debug)


# ---------------------------
# Setup Build
# ---------------------------
file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")

set(NETOFF_INCLUDE_DIRS_INTERNAL "${PROJECT_SOURCE_DIR}/include" ${SDL2_INCLUDE_DIRS} ${SDL2_NET_INCLUDE_DIRS})
set(NETOFF_LIBRARIES_INTERNAL ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES} "netoff")

# Netoff as shared library
add_library(netoff SHARED ${SRCS})
target_link_libraries(netoff ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES})
target_include_directories(netoff PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
set_property(TARGET netoff PROPERTY CXX_STANDARD 11)
set_property(TARGET netoff PROPERTY CXX_STANDARD_REQUIRED ON)

# Netoff as static library
add_library(netoffStatic STATIC ${SRCS})
target_link_libraries(netoffStatic ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES})
target_include_directories(netoffStatic PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
set_property(TARGET netoffStatic PROPERTY CXX_STANDARD 11)
set_property(TARGET netoffStatic PROPERTY CXX_STANDARD_REQUIRED ON)
if(UNIX)
  set_target_properties(netoffStatic PROPERTIES OUTPUT_NAME netoff)
endif(UNIX)

install(FILES "include/AdditionalTypes.hpp" DESTINATION "include/NetOff")
install(FILES "include/SimulationClient.hpp" DESTINATION "include/NetOff")
install(FILES "include/SimulationServer.hpp" DESTINATION "include/NetOff")
install(FILES "include/ValueContainer.hpp"   DESTINATION "include/NetOff")
install(FILES "include/VariableList.hpp"    DESTINATION "include/NetOff")
install(TARGETS netoff DESTINATION "lib")
install(TARGETS netoffStatic DESTINATION "lib")

# Example program SimpleSim
add_executable(SimpleSim "test/SimpleSim.cpp")
set_property(TARGET SimpleSim PROPERTY CXX_STANDARD 11)
set_property(TARGET SimpleSim PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET SimpleSim PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples")
add_custom_command(TARGET SimpleSim PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
                   "${CMAKE_CURRENT_SOURCE_DIR}/test/test_src.txt" "${CMAKE_BINARY_DIR}/examples")

install(FILES "test/test_src.txt" DESTINATION "${CMAKE_INSTALL_PREFIX}/examples/NetOff")

target_link_libraries(SimpleSim ${NETOFF_LIBRARIES_INTERNAL})
target_include_directories(SimpleSim PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
install(TARGETS SimpleSim DESTINATION "examples/NetOff")

# Test program NetOffBenchmark
add_executable(NetOffBenchmark "test/NetOffBenchmark.cpp")
set_property(TARGET NetOffBenchmark PROPERTY CXX_STANDARD 11)
set_property(TARGET NetOffBenchmark PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET NetOffBenchmark PROPERTY BUILD_TYPE RELEASE)
target_link_libraries(NetOffBenchmark ${NETOFF_LIBRARIES_INTERNAL})
target_include_directories(NetOffBenchmark PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
install(TARGETS NetOffBenchmark DESTINATION "examples/NetOffBenchmark")