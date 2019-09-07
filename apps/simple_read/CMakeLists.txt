find_package(RapidJSON)

add_library(boost_beast INTERFACE)
target_include_directories(boost_beast
  SYSTEM
  INTERFACE
    $ENV{BEAST_DIR}/include)

message("$ENV{BEAST_DIR}/include")

add_executable(simple_read
  src/main.cpp
  )
set_target_properties(simple_read PROPERTIES
  CXX_STANDARD 11)
target_include_directories(simple_read
  PRIVATE
    include
    RapidJSON::RapidJSON)
target_sources(simple_read
  PRIVATE
    src/session.cpp
    src/listener.cpp
    src/utility.cpp
  )
target_link_libraries(simple_read
  ${Boost_LIBRARIES}
  kinect
  boost_beast
  )
