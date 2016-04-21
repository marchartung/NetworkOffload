

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

include_directories("${PROJECT_SOURCE_DIR}/include" ${SDL2_INCLUDE_DIRS} ${SDL2_NET_INCLUDE_DIRS})

add_library( netoff SHARED ${SRCS} )
target_link_libraries(netoff ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES})
set_property(TARGET netoff PROPERTY CXX_STANDARD 11)
set_property(TARGET netoff PROPERTY CXX_STANDARD_REQUIRED ON)


add_library(netoffStatic STATIC ${SRCS})
target_link_libraries(netoffStatic ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES})
set_property(TARGET netoffStatic PROPERTY CXX_STANDARD 11)
set_property(TARGET netoffStatic PROPERTY CXX_STANDARD_REQUIRED ON)
if(UNIX)
	set_target_properties(netoffStatic PROPERTIES OUTPUT_NAME netoff)
endif(UNIX)

add_executable(SimpleSim "test/SimpleSim.cpp")
set_property(TARGET SimpleSim PROPERTY CXX_STANDARD 11)
set_property(TARGET SimpleSim PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(SimpleSim ${SDL2_LIBRARIES} ${SDL2_NET_LIBRARIES} "netoff")


install(FILES "include/AdditionalTypes.hpp" DESTINATION include)
install(FILES "include/SimulationClient.hpp" DESTINATION include)
install(FILES "include/SimulationServer.hpp" DESTINATION include)
install(FILES "include/ValueContainer.hpp"   DESTINATION include)
install(FILES "include/VariableList.hpp"    DESTINATION include)
install(TARGETS SimpleSim DESTINATION example)
install(TARGETS netoff DESTINATION lib)
install(TARGETS netoffStatic DESTINATION lib)