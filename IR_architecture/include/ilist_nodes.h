//
// Created by denist on 9/24/22.
//

#ifndef JIT_AOT_IN_VM_ILIST_NODES_H
#define JIT_AOT_IN_VM_ILIST_NODES_H

#include <unordered_set>
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

    typename std::unordered_set<Elt *>::iterator GetSuccBegin() {
        return succ_.begin();
    }

    typename std::unordered_set<Elt *>::iterator GetSuccEnd() {
        return succ_.end();
    }

    typename std::unordered_set<Elt *>::iterator GetPredecBegin() {
        return predec_.begin();
    }

    typename std::unordered_set<Elt *>::iterator GetPredecEnd() {
        return predec_.end();
    }

    typename std::unordered_set<Elt *>::const_iterator GetSuccBegin() const {
        return succ_.begin();
    }

    typename std::unordered_set<Elt *>::const_iterator GetSuccEnd() const {
        return succ_.end();
    }

    typename std::unordered_set<Elt *>::const_iterator GetPredecBegin() const {
        return predec_.begin();
    }

    typename std::unordered_set<Elt *>::const_iterator GetPredecEnd() const {
        return predec_.end();
    }

    void AddSucc(Elt *elt) {
        succ_.insert(elt);
    }

    void AddPredec(Elt *elt) {
        predec_.insert(elt);
    }

    void RemoveSucc(typename std::unordered_set<Elt *>::iterator it) {
        succ_.erase(it);
    }

    void RemovePredec(typename std::unordered_set<Elt *>::iterator it) {
        predec_.erase(it);
    }

    typename std::unordered_set<Elt *>::iterator FindSucc(Elt *elt) {
        return succ_.find(elt);
    }

    typename std::unordered_set<Elt *>::iterator FindPredec(Elt *elt) {
        return predec_.find(elt);
    }

    typename std::unordered_set<Elt *>::size_type PredecSize() const {
        return predec_.size();
    }

    typename std::unordered_set<Elt *>::size_type SuccSize() const {
        return succ_.size();
    }

    bool IsBBSucc(Elt *elt) {
        return succ_.contains(elt);
    }

    bool IsBBPredec(Elt *elt) {
        return predec_.contains(elt);
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
    std::unordered_set<Elt *> predec_;
    std::unordered_set<Elt *> succ_;
};

#endif //JIT_AOT_IN_VM_ILIST_NODES_H
