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

    void RunAlgoAndBuildDomTree(const BasicBlock *graph) {
        graph_ = graph;
        RunStep1();
        RunStep2AndStep3();
        RunStep4();
        BuildDomTree();
        vertexes.clear();
        bbs_info_.clear();
        graph = nullptr;
    }

private:
    void RunStep1();
    void DFS(const BasicBlock *v, const BasicBlock *parent, size_t &cur_idx);
    void RunStep2AndStep3();
    void RunStep4();
    void BuildDomTree();

    const BasicBlock *Eval(const BasicBlock *v);
    void Link(const BasicBlock *v_parent, const BasicBlock *v);
    void Compress(const BasicBlock *v);

    std::unordered_map<const BasicBlock *, BasicBlockInfo> bbs_info_;
    std::vector<const BasicBlock *> vertexes;
    const BasicBlock *graph_;

};
#endif //VM_DFS_H
