

#ifndef VM_IR_GRAPH_H
#define VM_IR_GRAPH_H

#include <list>
#include <unordered_map>
#include <queue>
#include <unordered_set>

#include "ilist_nodes.h"
#include "basic_block.h"

class IRFunction;

class IRGraph {
public:
    using sztype = size_t;

    IRGraph(BasicBlock *root = nullptr, IRFunction *func = nullptr);

    IRGraph(const IRGraph &rhs);
    IRGraph &operator=(const IRGraph &) = delete;

    IRGraph(IRGraph && other) : func_(std::move(other.func_)),
    bb_set_(std::move(other.bb_set_)), root_(std::move(other.root_)) {};

    IRGraph &operator=(IRGraph &&) = delete;

    // void CreateAndInsertBBBack(Instruction *start_instr, const std::string &label = "");

    bool IsBBsConnected(BasicBlock *bb, BasicBlock *bb_succ) const;
    void AddBBToBegin(BasicBlock *bb);

    bool IsBBInGraph(BasicBlock *bb) {
        return bb_set_.contains(bb);
    }

    void AddBBInGraph(BasicBlock *bb) {
        bb_set_.insert(bb);
    }

    void AddEdge(BasicBlock *from, BasicBlock *to) {
        if(!IsBBInGraph(from) || !IsBBInGraph(to)) {
            throw std::invalid_argument("Can't add an edge between vertices that don't exist in the graph");
        }

        if(from->SuccSize() == 2) {
            throw std::invalid_argument("Trying to add third successor at AddEdge method");
        }

        if(from->SuccSize() == 1 && *(from->GetSuccBegin()) == to) {
            throw std::invalid_argument("Trying to add existing edge at AddEdge method");
        }

        from->AddSucc(to);
        to->AddPredec(from);
    }

    void RemoveEdge(BasicBlock *from, BasicBlock *to) {
        if(!IsBBInGraph(from) || !IsBBInGraph(to)) {
            throw std::invalid_argument("Can't remove an edge between vertices that don't exist in the graph");
        }

        if(from->SuccSize() == 0) {
            throw std::invalid_argument("Trying to remove non-existing edge at RemoveEdge method");
        }


        for(auto it = from->GetSuccBegin(), end = from->GetSuccEnd(); it != end; ++it) {
            if(*it == to) {
                from->RemoveSucc(*it);
                to->RemovePredec(from);
                return;
            }
        }

        throw std::invalid_argument("Trying to remove non-existing edge at RemoveEdge method");
    }

    BasicBlock *GetRoot() {
        return root_;
    }

    const BasicBlock *GetRoot() const {
        return root_;
    }

protected:
    IRFunction *func_;
    std::unordered_set<BasicBlock*> bb_set_;
    BasicBlock *root_;
};

#endif // VM_IR_GRAPH_H