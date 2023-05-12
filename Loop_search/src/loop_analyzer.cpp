//
// Created by denist on 12/13/22.
//

#include <ranges>

#include "loop_analyzer.h"

void LoopAnalyzer::Run(BasicBlock *root) {
    // Build Dominator Tree
    DomTree{}.Run(root);

    std::unordered_set<BasicBlock *> grays;
    std::unordered_set<BasicBlock *> blacks;
    DFS(root, grays, blacks);
    grays.clear();
    blacks.clear();

    RPO rpo;
    rpo.Run(root);
    CollectLoops(rpo, blacks);
    blacks.clear();

    loop_tree_root_ = new Loop(idx_gen, nullptr, false);
    blacks.clear();
    CollectRootLoop(root, blacks);
    blacks.clear();
    AssignRootLoop(rpo);
}

void LoopAnalyzer::DFS(BasicBlock *node, std::unordered_set<BasicBlock*> &grays,
         std::unordered_set<BasicBlock*> &blacks) {
    grays.insert(node);
    blacks.insert(node);
    for(auto it = node->GetSuccBegin(); it != node->GetSuccEnd(); ++it) {
        if(grays.contains(*it)) {
            ProcessBackEdge(*it, node);
        } else if (!blacks.contains(*it)) {
            DFS(*it, grays, blacks);
        }
    }
    grays.erase(node);
}

void LoopAnalyzer::ProcessBackEdge(BasicBlock *header, BasicBlock *backEdge) {
    auto *loop = header->GetLoop();
    if(loop == nullptr) {
        loop = new Loop(idx_gen, header, header->IsDominated(backEdge));
        ++idx_gen;
        header->SetLoop(loop);
    }

    loop->insertBackEdge(backEdge);
}

void LoopAnalyzer::CollectLoops(RPO &rpo, std::unordered_set<BasicBlock*> &greens) {
    for(auto it = rpo.rbegin(), end = rpo.rend(); it != end; ++it) {
        // if it is not header
        if(!(*it)->GetLoop() || (*it)->GetLoop()->getHeader() != *it) {
            continue;
        }

        auto *loop = (*it)->GetLoop();
        if(loop->isReducible()) {
            greens.insert(*it);
            for(auto b_it = loop->backEdgeBegin(), b_end = loop->backEdgeEnd(); b_it != b_end; ++b_it) {
                ProcessLoopBBs(loop, *b_it, greens);
            }
        }

        greens.clear();
    }
}

void LoopAnalyzer::ProcessLoopBBs(Loop *cur_loop, BasicBlock *bb, std::unordered_set<BasicBlock*> &greens) {
    if(!greens.contains(bb)) {
        greens.insert(bb);

        if(bb->GetLoop() == nullptr) {
            cur_loop->insertLoopBB(bb);
        } else if(bb->GetLoop()->GetPredecBegin() == bb->GetLoop()->GetPredecEnd()
        && bb->GetLoop() != cur_loop) {
            cur_loop->AddSuccWithPredec(bb->GetLoop());
        }

        for(auto it = bb->GetPredecBegin(), end = bb->GetPredecEnd(); it != end; ++it) {
            ProcessLoopBBs(cur_loop, *it, greens);
        }
    }
}

void LoopAnalyzer::CollectRootLoop(BasicBlock *bb, std::unordered_set<BasicBlock *> &blacks) {
    if(!blacks.contains(bb)) {
        blacks.insert(bb);
        if(bb->GetLoop() == nullptr) {
            loop_tree_root_->insertLoopBB(bb);
        }
        for(auto it = bb->GetSuccBegin(), end = bb->GetSuccEnd(); it != end; ++it) {
            CollectRootLoop(*it, blacks);
        }
    }
}

void LoopAnalyzer::AssignRootLoop(RPO &rpo) {
    for(auto &it : rpo) {
        if(it->GetLoop()->getHeader() == it) {
            if(it->GetLoop()->GetPredecBegin() == it->GetLoop()->GetPredecEnd()) {
                loop_tree_root_->AddSuccWithPredec(it->GetLoop());
            }
        }
    }
}

Loop *LoopAnalyzer::GetLoopTreeRoot() const {
    return loop_tree_root_;
}
