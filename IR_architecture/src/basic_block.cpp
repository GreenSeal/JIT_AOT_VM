//
// Created by denist on 10/4/22.
//

#include "basic_block.h"

BasicBlock::BasicBlock(IRGraph *graph, const std::string &name, Instruction *start_inst) :
                        graph_(graph), name_(name), idom_(nullptr), loop_(nullptr) {
    if(start_inst == nullptr) {
        first_inst_ = nullptr;
        last_inst_ = nullptr;
        return;
    }

    first_inst_ = start_inst;
    while(start_inst->GetNext() != nullptr) {
        start_inst = start_inst->GetNext();
    }

    last_inst_ = start_inst;
}

BasicBlock::BasicBlock(const BasicBlock &rhs) {
    if(rhs.first_inst_ == nullptr) {
        first_inst_ = nullptr;
        last_inst_ = nullptr;
        return;
    }
    const Instruction *cur_inst = rhs.first_inst_;
    Instruction *prev_inst = nullptr;
    while(cur_inst != nullptr) {
        Instruction *new_instr = cur_inst->clone();
        new_instr->SetPrev(prev_inst);
        if(prev_inst != nullptr) {
            prev_inst->SetNext(new_instr);
        }
        if(cur_inst == rhs.first_inst_) {
            first_inst_ = new_instr;
        }

        prev_inst = new_instr;
        cur_inst = cur_inst->GetNext();
    }
    prev_inst->SetNext(nullptr);
    last_inst_ = prev_inst;

    succ_.clear();
    predec_.clear();
}

void BasicBlock::ReplaceInstBack(Instruction *inst) {
    if(inst == nullptr) {
        throw std::invalid_argument("Nullptr given in AddInstBack method of BasicBlock class");
    }

    if(first_inst_ == nullptr) {
        first_inst_ = last_inst_ = inst;
    } else {
        last_inst_->SetNext(inst);
        inst->SetPrev(last_inst_);
        last_inst_ = inst;
    }
}

void BasicBlock::SetFirstInst(Instruction *first_inst) {
    auto first_inst_old = first_inst_;
    first_inst_ = first_inst;
    first_inst_->SetNext(first_inst_old->GetNext());
    first_inst_->GetNext()->SetPrev(first_inst_);
    delete first_inst_old;
}

void BasicBlock::SetLastInst(Instruction *last_inst) {
    auto last_inst_old = last_inst_;
    last_inst_ = last_inst;
    last_inst_->SetPrev(last_inst_old->GetPrev());
    last_inst_->GetPrev()->SetNext(last_inst_);
    delete last_inst_old;
}
