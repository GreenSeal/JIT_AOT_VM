project(VM)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(IR_ARCHITECTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/IR_architecture)

add_subdirectory(IR_architecture)
add_subdirectory(Dom_tree)

# ============== Adding googletest library ===================
include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
#==============================================================