//
// Created by denist on 10/25/22.
//

#ifndef VM_DFS_H
#define VM_DFS_H
#include <unordered_map>

#include "DomTreeNode.h"
#include "basic_block.h"
#include "ir_graph.h"

class DomTree final {
public:

    DomTree(const IRGraph &graph) : graph_(graph) {}

    void Run() {
        RunStep1();
        RunStep2AndStep3();
        RunStep4();
    }

private:
    void RunStep1();
    void DFS(const BasicBlock *v, const BasicBlock *parent, size_t &cur_idx);
    void RunStep2AndStep3();
    void RunStep4();

    const BasicBlock *Eval(const BasicBlock *v);
    void Link(const BasicBlock *v_parent, const BasicBlock *v);
    void Compress(const BasicBlock *v);

    const BasicBlock *root_;
    std::unordered_map<const BasicBlock *, BasicBlockInfo> bbs_info_;
    std::vector<const BasicBlock *> vertexes;
    const IRGraph &graph_;

};
#endif //VM_DFS_H
