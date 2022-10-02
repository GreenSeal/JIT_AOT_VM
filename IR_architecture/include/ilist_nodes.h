//
// Created by denist on 9/24/22.
//

#ifndef JIT_AOT_IN_VM_ILIST_NODES_H
#define JIT_AOT_IN_VM_ILIST_NODES_H

#include <vector>

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
        if(idx < succ.size()) {
            return succ.at(idx);
        } else if(succ.size() != 0){
            return succ.at(succ.size()-1);
        } else {
            throw std::invalid_argument("Invalid successor idx given to GetNext of class ilist_graph_node");
        }
    }

    Elt *GetPrev(sztype idx) const {
        if(idx < predec.size()) {
            return predec.at(idx);
        } else if(predec.size() != 0){
            return predec.at(predec.size()-1);
        } else {
            throw std::invalid_argument("Invalid predecessor idx given to GetNext of class ilist_graph_node");
        }
    }

    void SetNext(Elt *elt) {
        succ.push_back(elt);
    }

    void SetNext(Elt *elt, sztype idx) {
        if(idx < succ.size()) {
            return succ[idx];
        } else {
            throw std::invalid_argument("Invalid successor idx given to SetNext of class ilist_graph_node");
        }
    }

    void SetPrev(Elt *elt) {
        predec.push_back(elt);
    }

    void SetPrev(Elt *elt, sztype idx) {
        if(idx < predec.size()) {
            return predec[idx];
        } else {
            throw std::invalid_argument("Invalid predecessor idx given to SetNext of class ilist_graph_node");
        }
    }

    typename std::vector<Elt *>::size_type PredecSize() const {
        return predec.size();
    }

    typename std::vector<Elt *>::size_type SuccSize() const {
        return succ.size();
    }

private:
    std::vector<Elt *> predec;
    std::vector<Elt *> succ;
};

#endif //JIT_AOT_IN_VM_ILIST_NODES_H
