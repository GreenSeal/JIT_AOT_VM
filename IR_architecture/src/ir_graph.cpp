//
// Created by denist on 10/4/22.
//

#include "ir_graph.h"

IRGraph::IRGraph(BasicBlock *root, IRFunction *func) : func_(func), root_(nullptr) {
    if(root != nullptr) {

        // first we go around all the vertices of the graph using BFS algorithm and copy them
        // ==================================================================================

        std::unordered_set<BasicBlock*> explored_bbs;
        std::queue<BasicBlock*> bbs_to_go_through;

        bbs_to_go_through.push(root);
        explored_bbs.insert(root);

        while(!bbs_to_go_through.empty()) {
            auto cur_bb = bbs_to_go_through.front();
            bbs_to_go_through.pop();

            bb_set_.insert(cur_bb);

            for(auto it = cur_bb->GetSuccBegin(), end = cur_bb->GetSuccEnd()/*size_t idx = 0*/; it != end; ++it) {
                if(!explored_bbs.contains(*it)) {
                    bbs_to_go_through.push(*it);
                    explored_bbs.insert(*it);
                }
            }
        }

        // ==================================================================================
        root_ = root;
    }
}

IRGraph::IRGraph(const IRGraph &rhs) {
    if(rhs.root_ != nullptr) {

        // first we go around all the vertices of the graph using BFS algorithm and copy them
        // ==================================================================================
        std::unordered_map<const BasicBlock*, BasicBlock*> old_to_new_map;

        std::unordered_set<const BasicBlock*> explored_old_bbs;
        std::queue<const BasicBlock*> bbs_to_copy;

        bbs_to_copy.push(rhs.root_);
        explored_old_bbs.insert(rhs.root_);

        while(!bbs_to_copy.empty()) {
            auto old_bb = bbs_to_copy.front();
            bbs_to_copy.pop();

            BasicBlock *new_bb = old_bb->clone();
            old_to_new_map[old_bb] = new_bb;
            bb_set_.insert(new_bb);

            for(auto it = old_bb->GetSuccBegin(), end = old_bb->GetSuccEnd(); it != end; ++it) {
                if(!explored_old_bbs.contains(*it)) {
                    bbs_to_copy.push(*it);
                    explored_old_bbs.insert(*it);
                }
            }
        }

        // ==================================================================================

        // then fill adjacency list using again BFS
        // ==================================================================================
        explored_old_bbs.clear();

        bbs_to_copy.push(rhs.root_);
        explored_old_bbs.insert(rhs.root_);

        while(!bbs_to_copy.empty()) {
            auto old_bb = bbs_to_copy.front();
            bbs_to_copy.pop();

            BasicBlock *new_bb = old_to_new_map.at(old_bb);
            for(auto it = old_bb->GetPredecBegin(), end = old_bb->GetPredecEnd(); it != end; ++it) {
                BasicBlock *old_pred = *it;
                BasicBlock *new_pred = old_to_new_map.at(old_pred);
                new_bb->AddPredec(new_pred);
            }

            for(auto it = old_bb->GetSuccBegin(), end = old_bb->GetSuccEnd(); it != end; ++it) {
                BasicBlock *old_succ = *it;
                BasicBlock *new_succ = old_to_new_map.at(old_succ);
                new_bb->AddSucc(new_succ);

                if(!explored_old_bbs.contains(old_succ)) {
                    bbs_to_copy.push(old_succ);
                    explored_old_bbs.insert(old_succ);
                }
            }
        }
        // ==================================================================================
        root_ = old_to_new_map.at(rhs.root_);
    }
}


/*void IRGraph::CreateAndInsertBBBack(InstructionBase *start_instr, const std::string &label) {
    auto *new_bb = new BasicBlock(this, label, start_instr);

    root_->AddSucc(new_bb);
    new_bb->AddPredec(root_);
}*/

bool IRGraph::IsBBsConnected(BasicBlock *bb, BasicBlock *bb_succ) const {
    return bb->IsBBSucc(bb_succ);
}

void IRGraph::AddBBToBegin(BasicBlock *bb) {
    if(root_ == nullptr) {
        root_ = bb;
        bb_set_.insert(root_);
        return;
    }

    bb->AddSucc(root_);
    root_->AddPredec(bb);
    root_ = bb;
    bb_set_.insert(root_);
}


