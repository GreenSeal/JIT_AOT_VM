add_executable(build_ir main.cpp)
target_include_directories(build_ir PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(google_tests)
