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

    using node_t = Elt;

    Elt *GetPrev() const {
        return prev_;
    }

    Elt *GetNext() const {
        return next_;
    }

    void AssignPrev(Elt *elt) {
        prev_ = elt;
    }

    void AssignNext(Elt *elt) {
        next_ = elt;
    }

    void AssignNextAndPrev(Elt *elt) {
        next_ = elt;
        elt->AssignPrev(static_cast<Elt *>(this));
    }

    virtual ~ilist_bidirectional_node() {}
protected:
    ilist_bidirectional_node() = default;
    ilist_bidirectional_node(const ilist_bidirectional_node &rhs) = default;

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
        elt->AddPredec(static_cast<Elt *>(this));
    }

    void RemoveSucc(Elt *elt) {
        if(!IsBBSucc(elt)) {
            throw std::invalid_argument("Trying to remove non-existing successor");
        }
        std::swap(elt, *succ_.rbegin());
        succ_.pop_back();
    }

    void RemovePredec(Elt *elt) {
        if(!IsBBPredec(elt)) {
            throw std::invalid_argument("Trying to remove non-existing predecessor");
        }
        std::swap(elt, *predec_.rbegin());
        predec_.pop_back();
    }

    void RemoveSuccAndPredec(Elt *elt) {
        RemoveSucc(elt);
        elt->RemovePredec(static_cast<Elt *>(this));
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
    ilist_graph_node() = default;
    ilist_graph_node(const ilist_graph_node &) = default;

    std::vector<Elt *> predec_;
    std::vector<Elt *> succ_;
};

#endif //JIT_AOT_IN_VM_ILIST_NODES_H
