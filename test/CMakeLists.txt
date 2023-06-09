add_executable(mallochook mallochook_test.cc)

add_executable(pingpong pingpong_test.cc)
target_link_libraries(pingpong luxbase)

add_executable(MTQueue MTQueue_test.cc)
target_link_libraries(MTQueue luxbase)

add_executable(exception exception_test.cc)
target_link_libraries(exception luxbase)


