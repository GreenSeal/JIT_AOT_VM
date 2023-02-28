project(VM)
cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(IR_ARCHITECTURE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/IR_architecture)
set(DOM_TREE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Dom_tree)

add_subdirectory(IR_architecture)
add_subdirectory(Dom_tree)
add_subdirectory(Loop_search)
add_subdirectory(Inlining)
if(NOT EXISTS ${dir}/CMakeLists.txt)
    execute_process(COMMAND git submodule update --init --recursive -- third_party/googletest)
endif()
add_subdirectory(third_party/googletest)

# ============== Adding googletest library ===================
#include(FetchContent)
#FetchContent_Declare(
#        googletest
#        GIT_REPOSITORY https://github.com/google/googletest.git
#        GIT_TAG release-1.12.1
#)
#
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
#FetchContent_MakeAvailable(googletest)
#==============================================================