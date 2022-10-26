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

    void Run(const IRGraph &graph) {
        RunStep1(graph);
        RunStep2AndStep3();
        RunStep4();
    }

private:
    void RunStep1(const IRGraph& graph);
    void DFS(const BasicBlock *v, const BasicBlock *parent, size_t &cur_idx);
    void RunStep2AndStep3();
    void RunStep4();

    const BasicBlock *Eval(const BasicBlock *v);
    void Link(const BasicBlock *v_parent, const BasicBlock *v);
    void Compress(const BasicBlock *v);

    const BasicBlock *root_;
    std::unordered_map<const BasicBlock *, BasicBlockInfo> bbs_info_;
    std::vector<const BasicBlock *> vertexes;
    std::

};
#endif //VM_DFS_H