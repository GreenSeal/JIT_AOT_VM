#include "ilist_nodes.h"

class RuntimeInfo {};
class MethodInfo {};
class BackendInfo {};
class OptimisationsInfo {};

class IRTreeStorage : public RuntimeInfo, MethodInfo, BackendInfo, OptimisationsInfo {

protected:
    std::vector<std::vector<BasicBlock>> adjacency_list_;
    std::vector<BasicBlock> verticles_; // bads
    BasicBlock* root_;
};

class IRTreeBuilder : public IRTreeStorage {
public:
    using sztype = size_t;

    void CreateBB(const InstructionBase& start_instr, const std::string &label = "") {
        BasicBlock new_bb(this, label);
        decltype(start_instr) inst = start_instr;
        while(inst != nullptr) {
            new_bb.AddInstrBack(inst);
            inst = *(inst.GetNext());
        }
        verticles_.push_back(new_bb);
    }

    void CreateBB(BasicBlock bb) {

    }

    void CreateFunction() {

    }

    void CreateMoviU64() {

    }

    void CreateU32ToU64() {

    }

    void CreateCmpU64() {

    }

    void CreateJa() {

    }

    void CreateMulU64() {

    }

    void CreateAddiU64() {

    }

    void CreateJmp() {

    }

    void CreateRetU64() {

    }

};