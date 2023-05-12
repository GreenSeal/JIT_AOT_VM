//
// Created by denist on 12/12/22.
//

#pragma once

#include "basic_block.h"
#include "DomTree.h"
#include "loop.h"
#include "RPO.h"

class LoopAnalyzer {
public:
    LoopAnalyzer() : loop_tree_root_(nullptr), idx_gen(0) {}
    void Run(BasicBlock *root);
    Loop *GetLoopTreeRoot() const;
protected:
    Loop *loop_tree_root_;
    size_t idx_gen;
private:
    void DFS(BasicBlock *node, std::unordered_set<BasicBlock*> &grays,
        std::unordered_set<BasicBlock*> &blacks);

    void ProcessBackEdge(BasicBlock *header, BasicBlock *backEdge);
    void CollectLoops(RPO &rpo, std::unordered_set<BasicBlock*> &greens);
    void ProcessLoopBBs(Loop *cur_loop, BasicBlock *bb, std::unordered_set<BasicBlock*> &greens);
    void CollectRootLoop(BasicBlock *bb, std::unordered_set<BasicBlock *> &blacks);
    void AssignRootLoop(RPO &rpo);
};
