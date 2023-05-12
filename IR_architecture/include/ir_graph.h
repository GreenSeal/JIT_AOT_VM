

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

    //IRGraph(BasicBlock *root = nullptr, IRFunction *func = nullptr);
    IRGraph(Instruction *first, IRFunction *func = nullptr);

    IRGraph(const IRGraph &rhs);
    IRGraph &operator=(const IRGraph &) = delete;

    IRGraph(IRGraph &&other) : func_(std::move(other.func_)),
    bb_set_(std::move(other.bb_set_)), root_(std::move(other.root_)) {};

    IRGraph &operator=(IRGraph &&) = delete;

    Instruction *GetFirstInst() {
        return first_;
    }

    const Instruction *GetFirstInst() const {
        return first_;
    }

    void AssignFirstInst(Instruction *inst) {
        first_ = inst;
    }

    Instruction *GetLastInst() {
        return last_;
    }

    const Instruction *GetLastInst() const {
        return last_;
    }

    void AssignLastInst(Instruction *inst) {
        last_ = inst;
    }

    // TODO: implement these methods
//    void InsertInstForward(Instruction *inst);
//    void InsertInstBack(Instruction *inst);

//    Instruction *ReplaceFirstInst();
//    Instruction *ReplaceLastInst();

    void RemoveNonCFGOrSideInst(Instruction *inst) {
        auto *parent_bb = inst->GetParentBB();
        if(parent_bb->size() == 1) {
            parent_bb->AssignFirstInst(nullptr);
            parent_bb->AssignLastInst(nullptr);
        } else if(parent_bb->GetFirstInst() == inst) {
            parent_bb->AssignFirstInst(inst->GetNext());
        } else if(parent_bb->GetLastInst() == inst) {
            parent_bb->AssignLastInst(inst->GetPrev());
        }
        if(inst == first_) {
            first_ = first_->GetNext();
        } else if(inst == last_) {
            // if instruction the last in the graph then we know it is the last in according bb
            last_ = last_->GetPrev();
        } else {
            auto *prev = inst->GetPrev();
            auto *next = inst->GetNext();
            prev->AssignNextAndPrev(next);
        }

        delete inst;
    }

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

        from->AddSuccWithPredec(to);
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
                from->RemoveSuccWithPredec(*it);
                return;
            }
        }

        throw std::invalid_argument("Trying to remove non-existing edge at RemoveEdge method");
    }

    // TODO: rewrite in terms of move semantic and with idea that bb is just external structure on list of instructions
    void SplitBBInTwo(BasicBlock *bb, Instruction *inst) {
        if(bb->GetLastInst() != inst) {
            auto *new_bb = new BasicBlock();

            auto *fst_succ = bb->GetFirstSucc();
            auto *snd_succ = bb->GetSecondSucc();

            if(fst_succ) {
                bb->RemoveSuccWithPredec(fst_succ);
                new_bb->AddSuccWithPredec(fst_succ);
            }
            if(snd_succ) {
                bb->RemoveSuccWithPredec(snd_succ);
                new_bb->AddSuccWithPredec(snd_succ);
            }
            bb->AddSuccWithPredec(new_bb);

            new_bb->AssignFirstInst(inst->GetNext());
            new_bb->AssignLastInst(bb->GetLastInst());
            bb->AssignLastInst(inst);

            Instruction *walker = new_bb->GetFirstInst();
            while(walker != nullptr) {
                walker->SetParentBB(new_bb);
                walker = walker->GetNext();
            }
        }
    }

    BasicBlock *GetRoot() {
        return root_;
    }

    const BasicBlock *GetRoot() const {
        return root_;
    }

    const IRFunction *GetIRFunction() const {
        return func_;
    }

    IRFunction *GetIRFunction() {
        return func_;
    }

    void SetIRFunction(IRFunction *func) {
        func_ = func;
    }

    virtual ~IRGraph() {
        Instruction *walker = first_;
        while(walker != nullptr) {
            auto *next = walker->GetNext();
            delete walker;
            walker = next;
        }

        for(auto &&elt: bb_set_) {
            delete elt;
        }
    }

private:
    void CreateIRGraphFromInstList(Instruction *first);

protected:
    IRFunction *func_;
    std::unordered_set<BasicBlock*> bb_set_;
    BasicBlock *root_;
    Instruction *first_;
    Instruction *last_;
};

#endif // VM_IR_GRAPH_H