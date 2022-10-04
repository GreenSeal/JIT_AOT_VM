#ifndef JIT_AOT_IN_VM_BASIC_BLOCK_H
#define JIT_AOT_IN_VM_BASIC_BLOCK_H

#include <string>
#include <exception>
#include "instructions.h"

class IRGraph;

class BasicBlock : public ilist_graph_node<BasicBlock> {
public:

    BasicBlock(IRGraph *graph = nullptr, const std::string &name = "", const InstructionBase *start_inst = nullptr) :
               graph_(graph), name_(name) {
        if(start_inst == nullptr) {
            first_inst_ = nullptr;
            last_inst_ = nullptr;
            return;
        } 
        const InstructionBase *cur_inst = start_inst;
        InstructionBase *prev_inst = nullptr;
        while(cur_inst != nullptr) {
            InstructionBase *new_instr = cur_inst->clone();
            new_instr->SetPrev(prev_inst);
            if(prev_inst != nullptr) {
                prev_inst->SetNext(new_instr);
            }
            if(cur_inst == start_inst) {
                first_inst_ = new_instr;
            }

            prev_inst = new_instr;
            cur_inst = cur_inst->GetNext();
        }
        prev_inst->SetNext(nullptr);
        last_inst_ = prev_inst;
    }

    BasicBlock(const BasicBlock &rhs) : BasicBlock(nullptr, rhs.name_, rhs.first_inst_) {}

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

    void AddInstBack(const InstructionBase *inst) {
        if(inst == nullptr) {
            throw std::invalid_argument("Nullptr given in AddInstBack method of BasicBlock class");
        }

        InstructionBase *new_inst = inst->clone();
        new_inst->SetPrev(nullptr);
        new_inst->SetNext(nullptr);
        if(first_inst_ == nullptr) {
            first_inst_ = last_inst_ = new_inst;
        } else {
            last_inst_->SetNext(new_inst);
            new_inst->SetPrev(last_inst_);
            last_inst_ = new_inst;
        }
    }

    const InstructionBase *GetFirstInst() const {
        return first_inst_;
    }

    void SetFirstInst(InstructionBase *first_inst) {
        auto first_inst_old = first_inst_;
        first_inst_ = first_inst->clone();
        first_inst_->SetNext(first_inst_old->GetNext());
        first_inst_->GetNext()->SetPrev(first_inst_);
        delete first_inst_old;
    }

    const InstructionBase *GetLastInst() const {
        return last_inst_;
    }

    void SetLastInst(InstructionBase *last_inst) {
        auto last_inst_old = last_inst_;
        last_inst_ = last_inst->clone();
        last_inst_->SetPrev(last_inst_old->GetPrev());
        last_inst_->GetPrev()->SetNext(last_inst_);
        delete last_inst_old;
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

protected:
    IRGraph *graph_;
    InstructionBase *first_inst_;
    InstructionBase *last_inst_;
    std::string name_;
};

#endif //JIT_AOT_IN_VM_BASIC_BLOCK_H