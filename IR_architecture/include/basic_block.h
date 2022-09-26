#ifndef JIT_AOT_IN_VM_BASIC_BLOCK_H
#define JIT_AOT_IN_VM_BASIC_BLOCK_H

#include "Instruction.h"

class BasicBlock : public ilist_tree_node<BasicBlock> {
public:
    BasicBlock(IRTreeStorage* graph = nullptr, const std::string &name = "",
               InstructionBase* first_instr = nullptr, InstructionBase* last_instr = nullptr) :
               graph_(graph), name_(name), first_instr_(first_instr), last_instr_(last_instr) {}

    void AddInstrBack(const InstuctionBase &instr) {
        auto* new_instr = new InstructionBase(instr);
        if(first_instr_ == nullptr) {
            first_instr_ = last_instr_ = new_instr;
        } else {
            last_instr_->SetNext(new_instr);
            new_instr->SetPrev(last_instr_);
            last_instr_ = new_instr;
        }
    }

    ~BasicBlock() {
        while(first_instr_ != nullptr) {
            auto* next = first_instr_->GetNext();
            delete first_instr_;
            first_instr_ = next;
        }
    }
private:
    IRTreeStorage* graph_;
    InstructionBase* first_instr_;
    InstructionBase* last_instr_;
    std::string name_;
};

#endif //JIT_AOT_IN_VM_BASIC_BLOCK_H