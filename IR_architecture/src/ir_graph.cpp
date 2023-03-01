//
// Created by denist on 10/4/22.
//

#include "ir_graph.h"

//IRGraph::IRGraph(BasicBlock *root, IRFunction *func) : func_(func), root_(nullptr) {
//    if(root != nullptr) {
//
//        // first we go around all the vertices of the graph using BFS algorithm and copy them
//        // ==================================================================================
//
//        std::unordered_set<BasicBlock*> explored_bbs;
//        std::queue<BasicBlock*> bbs_to_go_through;
//
//        bbs_to_go_through.push(root);
//        explored_bbs.insert(root);
//
//        while(!bbs_to_go_through.empty()) {
//            auto cur_bb = bbs_to_go_through.front();
//            bbs_to_go_through.pop();
//
//            bb_set_.insert(cur_bb);
//
//            for(auto it = cur_bb->GetSuccBegin(), end = cur_bb->GetSuccEnd()/*size_t idx = 0*/; it != end; ++it) {
//                if(!explored_bbs.contains(*it)) {
//                    bbs_to_go_through.push(*it);
//                    explored_bbs.insert(*it);
//                }
//            }
//        }
//
//        // ==================================================================================
//        root_ = root;
//    }
//}

IRGraph::IRGraph(Instruction *first, IRFunction *func) : func_(func) {
    CreateIRGraphFromInstList(first);
}

IRGraph::IRGraph(const IRGraph &rhs) {
    if(rhs.first_ == nullptr) {
        CreateIRGraphFromInstList(nullptr);
    }

    Instruction *first = nullptr;
    Instruction *prev = nullptr;
    Instruction *walker = rhs.first_;

    std::unordered_map<Instruction *, Instruction *> new_to_old_insts;
    std::unordered_map<Instruction *, Instruction *> old_to_new_insts;
    while(walker != nullptr) {
        Instruction *cloned = walker->clone();
        new_to_old_insts[cloned] = walker;
        old_to_new_insts[walker] = cloned;
        if(prev) {
            prev->AssignNextAndPrev(cloned);
        } else {
            first = walker;
        }
        walker = walker->GetNext();
        prev = cloned;
    }

    for(auto &elt: old_to_new_insts) {
        if(IsJumpInst(elt.first)) {
            auto *old_jump_dest = static_cast<Jump *>(elt.first)->GetInstToJump();
            static_cast<Jump *>(elt.second)->SetInstToJump(old_to_new_insts.at(old_jump_dest));
        }
    }

    CreateIRGraphFromInstList(first);
}

void IRGraph::CreateIRGraphFromInstList(Instruction *first) {
    size_t inst_cnt = 0;
    const size_t MAX_INST_IN_BB = 10;
    std::vector<BasicBlock *> bbs_in_order;

    Instruction *walker = first;
    Instruction *first_in_bb = walker;

    for(;walker != nullptr; walker = walker->GetNext()) {
        if(inst_cnt == 0)
            first_in_bb = walker;

        if(inst_cnt == MAX_INST_IN_BB || IsJumpInst(walker) || IsExitInst(walker) ||
        walker->GetNext() == nullptr || walker->GetNext()->IsJumpDest()) {
            auto *new_bb = new BasicBlock(first_in_bb, walker, "", this);
            bb_set_.insert(new_bb);
            bbs_in_order.push_back(new_bb);
            inst_cnt = 0;
        } else {
            inst_cnt++;
        }
    }

    for(size_t i = 0; i < bbs_in_order.size(); ++i) {
        auto *cur_bb = bbs_in_order[i];

        if(IsJumpInst(cur_bb->GetLastInst())) {
            auto *last_inst = cur_bb->GetLastInst();
            switch (last_inst->GetInstType()) {
                case inst_t::jmp: {
                    auto *jump = static_cast<Jump *>(last_inst);
                    cur_bb->AddSuccWithPredec(jump->GetInstToJump()->GetParentBB());
                    break;
                }
                case inst_t::ja: {
                    auto *ja = static_cast<Jump *>(last_inst);
                    cur_bb->AddSuccWithPredec(bbs_in_order.at(i+1));
                    cur_bb->AddSuccWithPredec(ja->GetInstToJump()->GetParentBB());
                    break;
                }
                default:;
            }
        } else if(IsExitInst(cur_bb->GetLastInst())) {
            // do nothing
        } else {
            if(i != bbs_in_order.size()-1) {
                cur_bb->AddSuccWithPredec(bbs_in_order[i+1]);
            }
        }
    }

    if(bbs_in_order.empty()) {
        root_ = nullptr;
        first_ = nullptr;
        last_ = nullptr;
    } else {
        root_ = bbs_in_order[0];
        first_ = bbs_in_order[0]->GetFirstInst();
        last_ = bbs_in_order[bbs_in_order.size()-1]->GetLastInst();
    }
}

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



