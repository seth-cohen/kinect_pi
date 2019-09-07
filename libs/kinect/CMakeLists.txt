# Note that headers are optional, and do not affect add_library, but they will not
# show up in IDEs unless they are listed in add_library
set(HEADER_LIST "${KinectOpenNI2_SOURCE_DIR}/libs/kinect/include/kinect.hpp")

add_library(kinect
  src/kinect.cpp
  ${HEADER_LIST}
)
target_include_directories(kinect
  PUBLIC
    include
)
target_link_libraries(kinect
  PUBLIC
    OpenNI2::OpenNI2
)
