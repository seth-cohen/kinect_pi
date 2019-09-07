add_executable(server
  src/main.cpp
)
target_link_libraries(server
  ${Boost_LIBRARIES}
  kinect
)
