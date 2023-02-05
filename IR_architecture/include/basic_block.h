#ifndef JIT_AOT_IN_VM_BASIC_BLOCK_H
#define JIT_AOT_IN_VM_BASIC_BLOCK_H

#include <string>
#include <exception>
#include "instructions.h"
#include "marker_mgr.h"

class IRGraph;
class Loop;

class BasicBlock : public ilist_graph_node<BasicBlock> {
public:

    BasicBlock(IRGraph *graph = nullptr, const std::string &name = "", InstructionBase *start_inst = nullptr);
    BasicBlock(const BasicBlock &rhs);

    template <typename elt>
    class bb_iterator : std::iterator<std::bidirectional_iterator_tag,
            elt, std::ptrdiff_t, elt *, elt &> {

        elt *cur_pos;

    public:
        explicit bb_iterator(elt *new_pos = nullptr) {
            cur_pos = new_pos;
        }

        bb_iterator &operator++() {
            cur_pos = cur_pos->GetNext();
            return *this;
        }

        bb_iterator operator++(int) {
            auto old_pos = *this;
            cur_pos = cur_pos->GetNext();
            return old_pos;
        }

        bb_iterator &operator--() {
            cur_pos = cur_pos->GetPrev();
            return *this;
        }

        bb_iterator operator--(int) {
            auto old_pos = *this;
            cur_pos = cur_pos->GetPrev();
            return old_pos;
        }

        bool operator==(bb_iterator other) const {
            return cur_pos == other.cur_pos;
        }

        bool operator!=(bb_iterator other) const {
            return cur_pos != other.cur_pos;
        }

        elt *operator*() const {
            return cur_pos;
        }
    };

    using iterator = bb_iterator<InstructionBase>;
    using const_iterator = bb_iterator<const InstructionBase>;

    iterator begin() {
        return iterator(first_inst_);
    }

    const_iterator begin() const {
        return const_iterator(first_inst_);
    }

    iterator end() {
        return iterator();
    }

    const_iterator end() const {
        return const_iterator();
    }

    void ReplaceInstBack(InstructionBase *inst);

    const InstructionBase *GetFirstInst() const {
        return first_inst_;
    }

    void SetFirstInst(InstructionBase *first_inst);
    void SetLastInst(InstructionBase *last_inst);

    const InstructionBase *GetLastInst() const {
        return last_inst_;
    }

    std::string GetBBName() const {
        return name_;
    }

    void SetBBName(const std::string &name) {
        name_ = name;
    }

    IRGraph *GetIRGraph() const {
        return graph_;
    }

    void SetIdom(BasicBlock *idom) {
        idom_ = idom;
    }

    BasicBlock *GetIdom() {
        return idom_;
    }

    void PushBackDominatedV(BasicBlock *bb) {
        dominated_bbs_.push_back(bb);
    }

    auto GetDominatedBegin() {
        return dominated_bbs_.begin();
    }

    auto GetDominatedEnd() {
        return dominated_bbs_.end();
    }

    bool IsDirectlyDominated(BasicBlock *bb) {
        for(auto &&dominated_bb : dominated_bbs_) {
            if(dominated_bb == bb) {
                return true;
            }
        }

        return false;
    }

    bool IsDominated(BasicBlock *bb) {
        if(IsDirectlyDominated(bb))
            return true;

        for(const auto &dominated_bb : dominated_bbs_) {
            if(dominated_bb->IsDominated(bb)) {
                return true;
            }
        }

        return false;
    }
    /*ideas: Insertion in any place of bb
             Instruction removing
             */

    virtual ~BasicBlock() {
        while(first_inst_ != nullptr) {
            auto* next = first_inst_->GetNext();
            delete first_inst_;
            first_inst_ = next;
        }
    }

    virtual BasicBlock *clone() const {
        return new BasicBlock(*this);
    }

    void SetMarkerAt(size_t idx, size_t value) {
        if(idx < markers_.markers.size()) {
            markers_.markers[idx] = value;
        } else {
            throw std::invalid_argument("Bad marker idx");
        }
    }

    void UnSetMarkerAt(size_t idx) {
        if(idx < markers_.markers.size()) {
            markers_.markers[idx] = 0;
        } else {
            throw std::invalid_argument("Bad marker idx");
        }
    }

    size_t GetMarkerAt(size_t idx) {
        if(idx < markers_.markers.size()) {
            return markers_.markers[idx];
        } else {
            throw std::invalid_argument("Bad marker idx");
        }
    }

    void SetLoop(Loop *loop) {
        loop_ = loop;
    }

    Loop *GetLoop() const {
        return loop_;
    }

protected:

    size_t id_;

    IRGraph *graph_;
    InstructionBase *first_inst_;
    InstructionBase *last_inst_;
    std::string name_;

    std::vector<BasicBlock *> dominated_bbs_;
    BasicBlock *idom_;

    Loop *loop_;

    MarkerMgr<4> markers_;
};

#endif //JIT_AOT_IN_VM_BASIC_BLOCK_H