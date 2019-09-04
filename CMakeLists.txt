cmake_minimum_required (VERSION 3.5)
set(CMAKE_MODULE_PATH /home/scohen/cmake)

project(KinectIOStream)

find_package(Boost REQUIRED COMPONENTS iostreams system thread)
find_package(OpenNI2 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(client
  client/source/main.cpp
)
target_link_libraries(client 
  ${Boost_LIBRARIES}
  Threads::Threads
)

add_executable(server
  server/source/main.cpp
  server/source/device/kinect.cpp 
)
target_link_libraries(server
  ${Boost_LIBRARIES}
  OpenNI2::OpenNI2
)
