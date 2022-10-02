#ifndef JIT_AOT_IN_VM_BASIC_BLOCK_H
#define JIT_AOT_IN_VM_BASIC_BLOCK_H

#include <string>
#include <exception>
#include "instructions.h"

class IRGraphStorage;

class BasicBlock : public ilist_graph_node<BasicBlock> {
public:
    BasicBlock(IRGraphStorage *graph = nullptr, const std::string &name = "", const InstructionBase *start_inst = nullptr) :
               graph_(graph), name_(name) {
        if(start_inst == nullptr) {
            first_inst_ = nullptr;
            last_inst_ = nullptr;
            return;
        } 
        
        InstructionBase *prev_inst = nullptr;
        while(start_inst != nullptr) {
            InstructionBase *new_instr = start_inst->clone();
            new_instr->SetPrev(prev_inst);
            if(prev_inst != nullptr) {
                prev_inst->SetNext(new_instr);
            }
            prev_inst = new_instr;
            start_inst = start_inst->GetNext();
        }
        prev_inst->SetNext(nullptr);
        last_inst_ = prev_inst;
    }

    void AddInstBack(const InstructionBase *inst) {
        if(inst == nullptr) {
            throw std::invalid_argument("Nullptr given in AddInstBack method of BasicBlock class");
        }

        InstructionBase *new_inst = inst->clone();
        new_inst->SetPrev(nullptr);
        new_inst->SetNext(nullptr);
        if(first_inst_ == nullptr) {
            first_inst_ = last_inst_ = new_inst;
        } else {
            last_inst_->SetNext(new_inst);
            new_inst->SetPrev(last_inst_);
            last_inst_ = new_inst;
        }
    }

    const InstructionBase *GetFirstInst() const {
        return first_inst_;
    }

    void SetFirstInst(InstructionBase *first_inst) {
        first_inst_ = first_inst->clone();
    }

    const InstructionBase *GetLastInst() const {
        return last_inst_;
    }

    void SetLastInst(InstructionBase *last_inst) {
        last_inst_ = last_inst->clone();
    }

    std::string GetBBLabel() const {
        return name_;
    }

    void SetBBName(const std::string &name) {
        name_ = name;
    }

    IRGraphStorage *GetIRGraph() const {
        return graph_;
    }

    /*ideas: Insertion in any place of bb
             Instruction removing
             */

    ~BasicBlock() {
        while(first_inst_ != nullptr) {
            auto* next = first_inst_->GetNext();
            delete first_inst_;
            first_inst_ = next;
        }
    }

protected:
    IRGraphStorage *graph_;
    InstructionBase *first_inst_;
    InstructionBase *last_inst_;
    std::string name_;
};

#endif //JIT_AOT_IN_VM_BASIC_BLOCK_H