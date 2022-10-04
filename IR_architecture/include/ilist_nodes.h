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

    Elt *GetNext(sztype idx) const {
        if(idx < succ_.size()) {
            return succ_.at(idx);
        } else if(succ_.size() != 0){
            return succ_.at(succ_.size()-1);
        } else {
            throw std::invalid_argument("Invalid successor idx given to GetNext of class ilist_graph_node");
        }
    }

    Elt *GetPrev(sztype idx) const {
        if(idx < predec_.size()) {
            return predec_.at(idx);
        } else if(predec_.size() != 0){
            return predec_.at(predec_.size()-1);
        } else {
            throw std::invalid_argument("Invalid predecessor idx given to GetNext of class ilist_graph_node");
        }
    }

    void PushBackSucc(Elt *elt) {
        succ_.push_back(elt);
    }

    void SetNext(Elt *elt, sztype idx) {
        if(idx < succ_.size()) {
            return succ_[idx];
        } else {
            throw std::invalid_argument("Invalid successor idx given to SetNext of class ilist_graph_node");
        }
    }

    void PushBackPredec(Elt *elt) {
        predec_.push_back(elt);
    }

    void SetPrev(Elt *elt, sztype idx) {
        if(idx < predec_.size()) {
            return predec_[idx];
        } else {
            throw std::invalid_argument("Invalid predecessor idx given to SetNext of class ilist_graph_node");
        }
    }

    typename std::vector<Elt *>::size_type PredecSize() const {
        return predec_.size();
    }

    typename std::vector<Elt *>::size_type SuccSize() const {
        return succ_.size();
    }

protected:
    std::vector<Elt *> predec_;
    std::vector<Elt *> succ_;
};

#endif //JIT_AOT_IN_VM_ILIST_NODES_H
