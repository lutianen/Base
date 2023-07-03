add_executable(mallochook mallochook_test.cc)

add_executable(MTQueue MTQueue_test.cc)
target_link_libraries(MTQueue Lute_Base)

add_executable(exception exception_test.cc)
target_link_libraries(exception Lute_Base)

add_executable(mutex mutex_test.cc)
target_link_libraries(mutex Lute_Base)

add_executable(currentThread currentThread_test.cc)
target_link_libraries(currentThread Lute_Base)

add_executable(utils utils_test.cc)
target_link_libraries(utils Lute_Base)

add_executable(fsUtils fsUtils_test.cc)
target_link_libraries(fsUtils Lute_Base)

add_executable(logger logger_test.cc)
target_link_libraries(logger Lute_Base)

add_executable(thread thread_test.cc)
target_link_libraries(thread Lute_Base pthread)

add_executable(any any_test.cc)
target_link_libraries(any Lute_Base)

add_executable(atomic atomic_test.cc)
target_link_libraries(atomic Lute_Base)

add_executable(string_view string_view_test.cc)
target_link_libraries(string_view Lute_Base)

add_executable(ini_config ini_config_test.cc)
target_link_libraries(ini_config Lute_Base)

add_executable(endian endian_test.cc)
target_link_libraries(endian Lute_Base)

add_executable(bytearray bytearray_test.cc)
target_link_libraries(bytearray Lute_Base)
