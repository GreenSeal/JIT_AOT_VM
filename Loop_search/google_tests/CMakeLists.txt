enable_testing()

add_executable(
        loop_analyzer_tests
        loop_analyzer_tests.cpp
)

target_include_directories(loop_analyzer_tests
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${IR_ARCHITECTURE_DIR}/include ${DOM_TREE_DIR}/include
        ${PROJECT_SOURCE_DIR}/RPO/include)

target_link_libraries(
        loop_analyzer_tests
        GTest::gtest_main
        loop_analyzer_lib
        IR_architecture_lib
        Dom_tree_algo
        RPO
)

include(GoogleTest)
gtest_discover_tests(loop_analyzer_tests)