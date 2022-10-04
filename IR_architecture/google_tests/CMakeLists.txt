enable_testing()

add_executable(
        IR_architecture_test
        IR_architecture_test.cpp
)

target_include_directories(IR_architecture_test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(
        IR_architecture_test
        GTest::gtest_main
        IR_architecture_lib
)

include(GoogleTest)
gtest_discover_tests(IR_architecture_test)