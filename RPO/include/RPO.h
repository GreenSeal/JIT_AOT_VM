#include <unordered_set>

#include "basic_block.h"

class RPO {
    void DFS(BasicBlock *node, std::unordered_set<BasicBlock*> &visited);
    std::vector<BasicBlock *> rpo_;
public:
    using iterator = std::vector<BasicBlock*>::iterator;

    void Run(BasicBlock *root);

    auto begin() {
        return rpo_.begin();
    }

    auto end() const {
        return rpo_.end();
    }

    auto rbegin() {
        return rpo_.rbegin();
    }

    auto rend() {
        return rpo_.rend();
    }
};
