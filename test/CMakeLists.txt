enable_testing()

add_executable(signals_test signals_test.cpp)
target_link_libraries(signals_test signals gtest)
add_test(NAME signals_test COMMAND signals_test)
