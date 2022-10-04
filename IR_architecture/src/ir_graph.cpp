//
// Created by denist on 10/4/22.
//

#include "ir_graph.h"

IRGraph::IRGraph(const BasicBlock *root, IRFunction *func) : func_(func), root_(nullptr), inserter_cursor_(nullptr) {
    if(root != nullptr) {

        // first we go around all the vertices of the graph using BFS algorithm and copy them
        // ==================================================================================
        std::unordered_map<const BasicBlock*, BasicBlock*> old_to_new_map;

        std::unordered_set<const BasicBlock*> explored_old_bbs;
        std::queue<const BasicBlock*> bbs_to_copy;

        bbs_to_copy.push(root);
        explored_old_bbs.insert(root);

        while(!bbs_to_copy.empty()) {
            auto old_bb = bbs_to_copy.front();
            bbs_to_copy.pop();

            BasicBlock *new_bb = old_bb->clone();
            old_to_new_map[old_bb] = new_bb;
            bb_set_.insert(new_bb);

            for(size_t idx = 0; idx < old_bb->SuccSize(); ++idx) {
                if(!explored_old_bbs.contains(old_bb->GetNext(idx))) {
                    bbs_to_copy.push(old_bb->GetNext(idx));
                    explored_old_bbs.insert(old_bb->GetNext(idx));
                }
            }
        }

        // ==================================================================================

        // then fill adjacency list using again BFS
        // ==================================================================================
        explored_old_bbs.clear();

        bbs_to_copy.push(root);
        explored_old_bbs.insert(root);

        while(!bbs_to_copy.empty()) {
            auto old_bb = bbs_to_copy.front();
            bbs_to_copy.pop();

            BasicBlock *new_bb = old_to_new_map.at(old_bb);
            for(size_t idx = 0; idx < old_bb->PredecSize(); ++idx) {
                BasicBlock *old_pred = old_bb->GetPrev(idx);
                BasicBlock *new_pred = old_to_new_map.at(old_pred);
                new_bb->PushBackPredec(new_pred);
            }

            for(size_t idx = 0; idx < old_bb->SuccSize(); ++idx) {
                BasicBlock *old_succ = old_bb->GetNext(idx);
                BasicBlock *new_succ = old_to_new_map.at(old_succ);
                new_bb->PushBackSucc(new_succ);

                if(!explored_old_bbs.contains(old_succ)) {
                    bbs_to_copy.push(old_succ);
                    explored_old_bbs.insert(old_succ);
                }
            }
        }
        // ==================================================================================
        root_ = old_to_new_map.at(root);
        inserter_cursor_ = root_;
    }
}

void IRGraph::CreateAndInsertBBBack(InstructionBase *start_instr, const std::string &label) {
    auto *new_bb = new BasicBlock(this, label, start_instr);

    root_->PushBackSucc(new_bb);
    new_bb->PushBackPredec(root_);
}

bool IRGraph::IsBBsConnected(BasicBlock *bb, BasicBlock *bb_succ) const {
    for(size_t idx = 0; idx < bb->SuccSize(); ++idx) {
        if(bb->GetNext(idx) == bb_succ)
            return true;
    }

    return false;
}

void IRGraph::PushBackSucc(BasicBlock *bb) {
    if(inserter_cursor_->SuccSize() >= 2) {
        throw std::invalid_argument("Trying to third successor to basic block at ir_graph");
    }

    if(bb_set_.contains(bb)) {
        inserter_cursor_->PushBackSucc(bb);
    } else {
        auto new_bb = bb->clone();
        bb_set_.insert(new_bb);
        inserter_cursor_->PushBackSucc(new_bb);

    }
}

void IRGraph::PushBackPred(BasicBlock *bb) {
    if(bb_set_.contains(bb)) {
        inserter_cursor_->PushBackPredec(bb);
    } else {
        auto new_bb = bb->clone();
        bb_set_.insert(new_bb);
        inserter_cursor_->PushBackPredec(new_bb);
    }
}

void IRGraph::AddBBToBegin(BasicBlock *bb) {
    if(root_ == nullptr) {
        root_ = bb->clone();
        bb_set_.insert(root_);
        return;
    }

    auto new_root = bb->clone();
    new_root->PushBackSucc(root_);
    root_->PushBackPredec(new_root);
    root_ = new_root;
    bb_set_.insert(root_);
}
