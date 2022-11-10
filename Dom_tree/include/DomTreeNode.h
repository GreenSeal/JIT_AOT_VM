//
// Created by denist on 10/25/22.
//

#ifndef VM_DOMTREENODE_H
#define VM_DOMTREENODE_H
#include "basic_block.h"

class BasicBlockInfo final {
public:

    BasicBlockInfo() : dom_(nullptr), parent_(nullptr) {}
    BasicBlockInfo(size_t id, size_t semi) :  dom_(nullptr), parent_(nullptr), id_(id), semi_(semi) {}

    void SetId(size_t v) {
        id_ = v;
    }

    size_t GetId() {
        return id_;
    }

    void SetParent(const BasicBlock *v) {
        parent_ = v;
    }

    const BasicBlock *GetParent() {
        return parent_;
    }

    void SetSemi(size_t semi) {
        semi_ = semi;
    }

    size_t GetSemi() {
        return semi_;
    }

    void SetDom(const BasicBlock *v) {
        dom_ = v;
    }

    const BasicBlock *GetDom() {
        return dom_;
    }

    void SetLabel(const BasicBlock *v) {
        label_ = v;
    }

    const BasicBlock *GetLabel() {
        return label_;
    }

    void SetAncestor(const BasicBlock *v) {
        ancestor_ = v;
    }

    const BasicBlock *GetAncestor() {
        return ancestor_;
    }

    void PushBackToBucket(const BasicBlock *v) {
        bucket_.push_back(v);
    }
    void PopBackFromBucket() {
        bucket_.pop_back();
    }

    const BasicBlock *GetLastInBucket() {
        return bucket_[bucket_.size()-1];
    }

    bool IsBucketEmpty() {
        return bucket_.empty();
    }

private:
    const BasicBlock *dom_;
    const BasicBlock *parent_;
    size_t id_;
    size_t semi_;
    const BasicBlock *label_;
    const BasicBlock *ancestor_;
    std::vector<const BasicBlock*> bucket_;


};


#endif //VM_DOMTREENODE_H
