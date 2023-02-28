//
// Created by denist on 10/4/22.
//

#include "basic_block.h"
#include "ir_graph.h"

BasicBlock::BasicBlock(Instruction *start_inst, Instruction *end_inst, const std::string &name, IRGraph *graph) :
                        graph_(graph), name_(name), idom_(nullptr), loop_(nullptr) {
    if(start_inst == nullptr) {
        first_inst_ = nullptr;
        last_inst_ = nullptr;
        return;
    }

    first_inst_ = start_inst;
    while(start_inst != nullptr) {
        start_inst->SetParentBB(this);
        start_inst = start_inst->GetNext();
    }

    last_inst_ = end_inst;
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
        new_instr->SetParentBB(this);
        new_instr->AssignPrev(prev_inst);
        if(prev_inst != nullptr) {
            prev_inst->AssignNext(new_instr);
        }
        if(cur_inst == rhs.first_inst_) {
            first_inst_ = new_instr;
        }

        prev_inst = new_instr;
        cur_inst = cur_inst->GetNext();
    }
    prev_inst->AssignNext(nullptr);
    last_inst_ = prev_inst;
}

Instruction *BasicBlock::ReplaceFirstInst(Instruction *first_inst) {
    auto first_inst_old = first_inst_;
    first_inst->SetParentBB(this);
    first_inst_ = first_inst;
    first_inst_->AssignNext(first_inst_old->GetNext());
    first_inst_->GetNext()->AssignPrev(first_inst_);
    if(graph_ != nullptr && graph_->GetFirstInst() == first_inst_old) {
        graph_->AssignFirstInst(first_inst_);
    }
    return first_inst_old;
}

Instruction *BasicBlock::ReplaceLastInst(Instruction *last_inst) {
    auto last_inst_old = last_inst_;
    last_inst->SetParentBB(this);
    last_inst_ = last_inst;
    last_inst_->AssignPrev(last_inst_old->GetPrev());
    last_inst_->GetPrev()->AssignNext(last_inst_);
    if(graph_ != nullptr && graph_->GetLastInst() == last_inst_old) {
        graph_->AssignLastInst(last_inst_);
    }
    return last_inst_old;
}

void BasicBlock::InsertInstForward(Instruction *inst) {
    inst->SetParentBB(this);
    if(first_inst_ == nullptr) {
        first_inst_ = last_inst_ = inst;
    } else {
        first_inst_->AssignPrev(inst);
        inst->AssignNext(first_inst_);
        first_inst_ = inst;
    }

    if(graph_ != nullptr && graph_->GetFirstInst() == first_inst_->GetNext()) {
        graph_->AssignFirstInst(first_inst_);
    }
}

void BasicBlock::InsertInstBack(Instruction *inst) {
    inst->SetParentBB(this);
    if(first_inst_ == nullptr) {
        first_inst_ = last_inst_ = inst;
    } else {
        last_inst_->AssignNext(inst);
        inst->AssignPrev(last_inst_);
        last_inst_ = inst;
    }

    if(graph_ != nullptr && graph_->GetLastInst() == last_inst_->GetPrev()) {
        graph_->AssignLastInst(last_inst_);
    }
}
