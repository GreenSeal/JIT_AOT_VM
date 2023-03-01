#ifndef JIT_AOT_IN_VM_BASIC_BLOCK_H
#define JIT_AOT_IN_VM_BASIC_BLOCK_H

#include <string>
#include <exception>
#include "instructions.h"
#include "marker_mgr.h"

class IRGraph;
class Loop;

inline bool IsJumpInst(const Instruction *inst) {
    switch(inst->GetInstType()) {
        case inst_t::jmp:
        case inst_t::ja:
            return true;
        default: return false;
    }
}

inline bool IsExitInst(const Instruction *inst) {
    switch (inst->GetInstType()) {
        case inst_t::ret:
            return true;
        default: return false;
    }
}

class BasicBlock : public ilist_graph_node<BasicBlock> {
public:

    BasicBlock(Instruction *start_inst = nullptr, Instruction *end_inst = nullptr, const std::string &name = "", IRGraph *graph = nullptr);

    BasicBlock(const BasicBlock &rhs);
    BasicBlock &operator=(const BasicBlock &) = delete;

    BasicBlock(BasicBlock &&) = delete;
    BasicBlock &operator=(BasicBlock &&) = delete;

    template <class InstPtr>
    class bb_iterator {

        InstPtr cur_pos;

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Instruction *;
        using difference_type = Instruction *;
        using pointer = Instruction **;
        using reference = Instruction *&;


        explicit bb_iterator(InstPtr new_pos = nullptr) {
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

        InstPtr operator*() const {
            return cur_pos;
        }

        InstPtr operator->() const {
            return cur_pos;
        }
    };

    using iterator = bb_iterator<Instruction *>;
    using const_iterator = bb_iterator<const Instruction *>;

    iterator begin() {
        return iterator{first_inst_};
    }

    const_iterator begin() const {
        return const_iterator{first_inst_};
    }

    iterator end() {
        return iterator();
    }

    const_iterator end() const {
        return const_iterator();
    }

    void InsertInstBack(Instruction *inst);
    void InsertInstForward(Instruction *inst);

    const Instruction *GetFirstInst() const {
        return first_inst_;
    }

    Instruction *GetFirstInst() {
        return first_inst_;
    }

    void AssignFirstInst(Instruction *first_inst) {
        first_inst_ = first_inst;
    }
    void AssignLastInst(Instruction *last_inst) {
        last_inst_ = last_inst;
    };

    Instruction *ReplaceFirstInst(Instruction *first_inst);
    Instruction *ReplaceLastInst(Instruction *last_inst);

    const Instruction *GetLastInst() const {
        return last_inst_;
    }

    Instruction *GetLastInst() {
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

    BasicBlock *clone() const {
        return new BasicBlock(*this);
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

    virtual ~BasicBlock() = default;

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

    size_t size() const {
        if(first_inst_ == nullptr) {
            return 0;
        } else if(first_inst_ == last_inst_) {
            return 1;
        }

        Instruction *walker = first_inst_;
        size_t count = 0;
        while(walker != last_inst_) {
            ++count;
            walker = walker->GetNext();
        }
        ++count;

        return count;
    }

    bool empty() const {
        if(first_inst_ == nullptr) return true;
        else return false;
    }

protected:

    size_t id_;

    IRGraph *graph_;
    Instruction *first_inst_;
    Instruction *last_inst_;
    std::string name_;

    std::vector<BasicBlock *> dominated_bbs_;
    BasicBlock *idom_;

    Loop *loop_;

    MarkerMgr<4> markers_;
};

#endif //JIT_AOT_IN_VM_BASIC_BLOCK_H