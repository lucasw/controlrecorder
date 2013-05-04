cmake_minimum_required(VERSION 2.4.6)
cmake_policy(SET CMP0003 OLD)

set(PROJECT_NAME controlrecorder)

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build )
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build )

include_directories( . )
#include_directories( $ENV{HG_CLONE_DIR}/external/include )
include_directories( $ENV{HOME}/other/install/include )
link_directories($ENV{HOME}/other/install/lib)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")

###############################################################################

find_package(OpenCV)
###############################################################################

add_executable(${PROJECT_NAME}
main.cpp
)

target_link_libraries(${PROJECT_NAME} 
${OpenCV_LIBS}
glog
gflags
boost_thread
boost_filesystem
boost_system
)


