//
// Created by denist on 10/4/22.
//

#include "basic_block.h"

BasicBlock::BasicBlock(IRGraph *graph, const std::string &name, const InstructionBase *start_inst) :
                        graph_(graph), name_(name) {
    if(start_inst == nullptr) {
        first_inst_ = nullptr;
        last_inst_ = nullptr;
        return;
    }
    const InstructionBase *cur_inst = start_inst;
    InstructionBase *prev_inst = nullptr;
    while(cur_inst != nullptr) {
        InstructionBase *new_instr = cur_inst->clone();
        new_instr->SetPrev(prev_inst);
        if(prev_inst != nullptr) {
            prev_inst->SetNext(new_instr);
        }
        if(cur_inst == start_inst) {
            first_inst_ = new_instr;
        }

        prev_inst = new_instr;
        cur_inst = cur_inst->GetNext();
    }
    prev_inst->SetNext(nullptr);
    last_inst_ = prev_inst;
}

void BasicBlock::AddInstBack(const InstructionBase *inst) {
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

void BasicBlock::SetFirstInst(InstructionBase *first_inst) {
    auto first_inst_old = first_inst_;
    first_inst_ = first_inst->clone();
    first_inst_->SetNext(first_inst_old->GetNext());
    first_inst_->GetNext()->SetPrev(first_inst_);
    delete first_inst_old;
}

void BasicBlock::SetLastInst(InstructionBase *last_inst) {
    auto last_inst_old = last_inst_;
    last_inst_ = last_inst->clone();
    last_inst_->SetPrev(last_inst_old->GetPrev());
    last_inst_->GetPrev()->SetNext(last_inst_);
    delete last_inst_old;
}
