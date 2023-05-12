//
// Created by denist on 12/13/22.
//

#pragma once
#include "ilist_nodes.h"

class BasicBlock;

class Loop final : public ilist_graph_node<Loop> {
public:
    Loop(size_t id, BasicBlock *header, bool is_reducible) : id_(id),
    header_(header), is_reducible_(is_reducible) {}

//    void pushLoopBB(BasicBlock *bb) {
//        loop_bbs_.push_back(bb);
//        bb->SetLoop(this);
//    }
//
//    void pushBackEdge(BasicBlock *bb) {
//        back_bbs_.push_back(bb);
//        pushLoopBB(bb);
//    }

    void insertLoopBB(BasicBlock *bb) {
        loop_bbs_.insert(bb);
        bb->SetLoop(this);
    }

    void insertBackEdge(BasicBlock *bb) {
        back_bbs_.insert(bb);
        insertLoopBB(bb);
    }

    BasicBlock *getHeader() const {
        return header_;
    }
    size_t getLoopBBSize() const {
        return loop_bbs_.size();
    }

//    BasicBlock *getLoopBBAt(size_t i) const {
//        return loop_bbs_.at(i);
//    }

    auto loopBBBegin() const {
        return loop_bbs_.begin();
    }

    auto loopBBEnd() const {
        return loop_bbs_.end();
    }

    size_t getBackEdgeSize() const {
        return back_bbs_.size();
    }

//    BasicBlock *getBackEdgeAt(size_t idx) const {
//        return back_bbs_.at(idx);
//    }

    bool isLoopContainsBB(BasicBlock *bb) {
        return loop_bbs_.contains(bb) || bb == header_;
    }

    bool isBBBackEdge(BasicBlock *bb) {
        return back_bbs_.contains(bb);
    }

    auto backEdgeBegin() const {
        return back_bbs_.begin();
    }

    auto backEdgeEnd() const {
        return back_bbs_.end();
    }

    bool isReducible() const {
        return is_reducible_;
    }

    size_t getID() const {
        return id_;
    }
private:
    size_t id_;
    std::unordered_set<BasicBlock*> loop_bbs_;
    std::unordered_set<BasicBlock*> back_bbs_;
    BasicBlock *header_;
    bool is_reducible_;
};
