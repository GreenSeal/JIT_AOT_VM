include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
add_library(IR_architecture_lib basic_block.cpp ir_function.cpp ir_graph.cpp ../include/operands.h)