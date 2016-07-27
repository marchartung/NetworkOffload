

cmake_minimum_required(VERSION 3.1.0 FATAL_ERROR)
project (NetworkOffloader)

SET(CMAKE_VERBOSE_MAKEFILE ON)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

find_package(SDL2)

if(NOT(SDL2_FOUND))
	message(FATAL_ERROR "SDL2 required")
endif(NOT(SDL2_FOUND))


find_package(SDL2_net)
if(NOT(SDL2_NET_FOUND))
	message(FATAL_ERROR "SDL2_net required")
endif(NOT(SDL2_NET_FOUND))


file(GLOB_RECURSE SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")

SET(NETOFF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/include" ${SDL2_INCLUDE_DIR} ${SDL2_NET_INCLUDE_DIRS})
SET(NETOFF_INCLUDE_DIRS_INTERNAL "${PROJECT_SOURCE_DIR}/include" ${SDL2_INCLUDE_DIR} ${SDL2_NET_INCLUDE_DIRS})

add_library( netoff STATIC ${SRCS} )
target_link_libraries(netoff ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES})
target_include_directories(netoff PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
set_property(TARGET netoff PROPERTY CXX_STANDARD 11)
set_property(TARGET netoff PROPERTY CXX_STANDARD_REQUIRED ON)


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

add_executable(SimpleSim "test/SimpleSim.cpp")
set_property(TARGET SimpleSim PROPERTY CXX_STANDARD 11)
set_property(TARGET SimpleSim PROPERTY CXX_STANDARD_REQUIRED ON)

set(NETOFF_LIBRARIES ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES} "netoff" )
set(NETOFF_LIBRARIES_INTERNAL ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES} "netoff")

target_link_libraries(SimpleSim ${NETOFF_LIBRARIES_INTERNAL})
target_include_directories(SimpleSim PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
install(TARGETS SimpleSim DESTINATION "example/NetOff")
file(COPY "${PROJECT_SOURCE_DIR}/test/test_src.txt" DESTINATION ".")

add_executable(NetOffBenchmark "test/NetOffBenchmark.cpp")
set_property(TARGET NetOffBenchmark PROPERTY CXX_STANDARD 11)
set_property(TARGET NetOffBenchmark PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET NetOffBenchmark PROPERTY BUILD_TYPE RELEASE)
target_link_libraries(NetOffBenchmark ${NETOFF_LIBRARIES_INTERNAL})
target_include_directories(NetOffBenchmark PUBLIC ${NETOFF_INCLUDE_DIRS_INTERNAL})
install(TARGETS NetOffBenchmark DESTINATION "example/NetOffBenchmark")
