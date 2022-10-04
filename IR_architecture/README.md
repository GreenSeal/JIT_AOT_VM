## IR classes decription
Project contains 5 entity: operands, instructions, basic_blocks, ir_graphs and ir_functions

### Operands
For operands, a system of inheriting classes has been developed. This allows you to make the code easily extensible, 
as well as support dynamic polymorphism. All operands have getters and setters, as well as enam to store the real class 
of the operand. There is also a virtual clone method that allows you to copy operands without using dynamic_cast
### Instructions
For instructions, as well as for operands, a system of inheriting classes has been developed. Classes for instructions 
are divided by the number of operands they take. Operands stored as unique_ptrs. Instructions also have getter and setters 
and methods clone. Also they have two types of constructors: one directly move std::unique_ptr<OparendBase> to class fields,
another initialize unique_ptrs with given pointer. Instructions inherited from ilist_bidirectional_node<Instruction>, which
сontains methods for walking through the list of given template argument.
### Basic blocks
Basic block contains pointers to the first and the last instruction. Also it has pointer to IRGraph, owning the basic block,
and the name of the label, because basic block might start with label. BasicBlock class has constructor, which copy list of 
instructions, starting with the given `root` pointer. (!)In this way, basic block doesn't own instruction list, given to the
constructor, it just copy all the instruction list. Basic block also contains list iterator, inherited from std::iterator,
so you can use algorithms from stl over the basic block. You can also push instruction back to the basic block through
the method `AddInstBack`, which also creates new pointer to the instruction, copy content of the given instruction and insert it
to the back of the instruction list.
### IR Graph
IRGraph implements the CFG abstraction. This class has pointer to the root basic block, inserter_cursor, with which you can
walk around the graph and change its edges, std::set<BasicBlock*>, to quickly determine whether the base block passed 
to the graph methods exists in the graph, and pointer to the ir function, owning the graph. Just like the base block constructor, 
the IRGraph constructor copies the CFG passed through the pointer to its root. IRGraph has methods that create all 
the instructions specified in the example, however, their list can be easily expanded. Also IRGraph has methods for walking
on the graph, checking is the block contained in the graph and changing edges and vertexes.
### IRFunction
IRFunction class contains body of the ir function, function arguments and function name.

## Testing
Framework Google Tests used for testing. All tests for IR architecture lies in the google_tests folder and have 5 test suites
which tests all the entity.

