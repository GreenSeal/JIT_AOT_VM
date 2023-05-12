
message("DOMTREE DIR = ${DOM_TREE_DIR}/include")
include_directories(loop_analyzer_lib
                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                ${IR_ARCHITECTURE_DIR}/include ${PROJECT_SOURCE_DIR}/RPO/include
                ${DOM_TREE_DIR}/include)
add_library(loop_analyzer_lib loop_analyzer.cpp)
#target_include_directories(loop_analyzer_lib
#        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
#        PUBLIC ${IR_ARCHITECTURE_DIR}/include PUBLIC ${DOM_TREE_DIR}/include
#        PUBLIC ${PROJECT_SOURCE_DIR}/RPO/include)
