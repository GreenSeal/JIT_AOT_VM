//
// Created by denist on 9/24/22.
//

#ifndef JIT_AOT_IN_VM_ILIST_NODES_H
#define JIT_AOT_IN_VM_ILIST_NODES_H

#include <vector>
#include <iterator>

template <typename Elt>
class ilist_bidirectional_node {
public:

    Elt *GetPrev() const {
        return prev_;
    }

    Elt *GetNext() const {
        return next_;
    }

    void SetPrev(Elt *elt) {
        prev_ = elt;
    }

    void SetNext(Elt *elt) {
        next_ = elt;
    }

    virtual ~ilist_bidirectional_node() {}
private:
    Elt *next_;
    Elt *prev_;
};

template <typename Elt>
class ilist_graph_node {
public:
    using sztype = size_t;

    typename std::vector<Elt *>::iterator GetSuccBegin() {
        return succ_.begin();
    }

    typename std::vector<Elt *>::iterator GetSuccEnd() {
        return succ_.end();
    }

    typename std::vector<Elt *>::iterator GetPredecBegin() {
        return predec_.begin();
    }

    typename std::vector<Elt *>::iterator GetPredecEnd() {
        return predec_.end();
    }

    typename std::vector<Elt *>::const_iterator GetSuccBegin() const {
        return succ_.begin();
    }

    typename std::vector<Elt *>::const_iterator GetSuccEnd() const {
        return succ_.end();
    }

    typename std::vector<Elt *>::const_iterator GetPredecBegin() const {
        return predec_.begin();
    }

    typename std::vector<Elt *>::const_iterator GetPredecEnd() const {
        return predec_.end();
    }

    void AddSucc(Elt *elt) {
        succ_.push_back(elt);
    }

    void AddPredec(Elt *elt) {
        predec_.push_back(elt);
    }

    void AddSuccWithPredec(Elt *elt) {
        succ_.push_back(elt);
        (succ_.rend()).AddPredec(this);
    }

    void RemoveSucc(Elt *elt) {
        if(!IsBBSucc(elt)) {
            throw std::invalid_argument("Trying to remove non-existing successor");
        }
        std::swap(elt, *succ_.rend());
        succ_.pop_back();
    }

    void RemovePredec(Elt *elt) {
        if(!IsBBPredec(elt)) {
            throw std::invalid_argument("Trying to remove non-existing predecessor");
        }
        std::swap(elt, *predec_.rend());
        predec_.pop_back();
    }

    typename std::vector<Elt *>::size_type PredecSize() const {
        return predec_.size();
    }

    typename std::vector<Elt *>::size_type SuccSize() const {
        return succ_.size();
    }

    bool IsBBSucc(Elt *elt) {
        for(auto &&node : succ_) {
            if(node == elt) return true;
        }
        return false;
    }

    bool IsBBPredec(Elt *elt) {
        for(auto &&node : predec_) {
            if(node == elt) return true;
        }

        return false;
    }

    Elt *GetFirstSucc() {
        if(succ_.size() < 1) {
            return nullptr;
        }
        return *succ_.begin();
    }

    const Elt *GetFirstSucc() const {
        if(succ_.size() < 1) {
            return nullptr;
        }
        return *succ_.begin();
    }

    Elt *GetSecondSucc() {
        if(succ_.size() < 2) {
            return nullptr;
        }

        return *(++succ_.begin());
    }

    const Elt *GetSecondSucc() const {
        if(succ_.size() < 2) {
            return nullptr;
        }

        return *(++succ_.begin());
    }

protected:
    std::vector<Elt *> predec_;
    std::vector<Elt *> succ_;
};

#endif //JIT_AOT_IN_VM_ILIST_NODES_H
