cmake_minimum_required (VERSION 3.5)
set(CMAKE_MODULE_PATH /home/scohen/cmake)

project(KinectOpenNI2
  VERSION 0.1
  LANGUAGES CXX)

find_package(Boost REQUIRED COMPONENTS iostreams system thread)
find_package(OpenNI2 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(libs/kinect)
add_subdirectory(apps/client)
add_subdirectory(apps/server)
add_subdirectory(apps/simple_read)


