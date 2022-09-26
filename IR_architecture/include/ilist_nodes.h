//
// Created by denist on 9/24/22.
//

#ifndef JIT_AOT_IN_VM_ILIST_NODES_H
#define JIT_AOT_IN_VM_ILIST_NODES_H

template <typename Elt>
class ilist_bidirectional_nodes {
public:
    Elt GetNext() const {
        return next;
    }

    Elt GetPrev const () {
        return prev;
    }

    void SetPrev(Elt* elt) {
        prev = elt;
    };

    virtual ~ilist_bidirectional_nodes() {}
private:
    Elt* next;
    Elt* prev;
};

template <typename Elt>
class ilist_tree_node {

};

#endif //JIT_AOT_IN_VM_ILIST_NODES_H
