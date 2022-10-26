enable_testing()

add_executable(
        Dom_tree_test
        Dom_tree_test.cpp
)

target_include_directories(Dom_tree_test
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${IR_ARCHITECTURE_DIR}/include)

target_link_libraries(
        Dom_tree_test
        GTest::gtest_main
        Dom_tree_algo
        IR_architecture_lib
)

include(GoogleTest)
gtest_discover_tests(Dom_tree_test)