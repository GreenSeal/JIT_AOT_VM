#include "RPO.h"

void RPO::DFS(BasicBlock *node, std::unordered_set<BasicBlock *> &visited) {
    visited.insert(node);

    for(auto it = node->GetSuccBegin(); it != node->GetSuccEnd(); ++it) {
        if(!visited.contains(*it)) {
            DFS(*it, visited);
        }
    }

    rpo_.push_back(node);
}

void RPO::Run(BasicBlock *root) {
    std::unordered_set<BasicBlock *> visited;
    DFS(root, visited);
    std::reverse(rpo_.begin(), rpo_.end());
}
