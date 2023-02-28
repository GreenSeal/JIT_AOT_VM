enable_testing()

add_executable(
        Static_inline_test
        Static_inline_test.cpp
)

target_include_directories(Static_inline_test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${IR_ARCHITECTURE_DIR}/include)

target_link_libraries(
        Static_inline_test
        GTest::gtest_main
        Static_inline_algo
        IR_architecture_lib
)

include(GoogleTest)
gtest_discover_tests(Static_inline_test)