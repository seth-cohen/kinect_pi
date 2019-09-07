add_executable(client
  src/main.cpp
)
target_link_libraries(client 
  ${Boost_LIBRARIES}
  Threads::Threads
)
