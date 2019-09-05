cmake_minimum_required (VERSION 3.5)
set(CMAKE_MODULE_PATH /home/scohen/cmake)

project(KinectOpenNI2)

find_package(Boost REQUIRED COMPONENTS iostreams system thread)
find_package(OpenNI2 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_library(kinect
  lib/source/kinect.cpp
)
target_include_directories(kinect
  PUBLIC
    lib/source
)
target_link_libraries(kinect
  PUBLIC
    OpenNI2::OpenNI2
)

add_executable(client
  client/source/main.cpp
)
target_link_libraries(client 
  ${Boost_LIBRARIES}
  Threads::Threads
)

add_executable(server
  server/source/main.cpp
)
target_link_libraries(server
  ${Boost_LIBRARIES}
  kinect
)
