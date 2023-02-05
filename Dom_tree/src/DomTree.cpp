//
// Created by denist on 10/26/22.
//

#include "DomTree.h"

void DomTree::RunStep1() {
    size_t cur_idx = 0;
    BasicBlockInfo root_info_(0, 0);
    cur_idx++;
    vertexes.push_back(graph_);
    bbs_info_.insert({graph_, std::move(root_info_)});
    for(auto it = graph_->GetSuccBegin(), end = graph_->GetSuccEnd(); it != end; ++it) {
        DFS(*it, graph_, cur_idx);
    }
}

void DomTree::DFS(const BasicBlock *v, const BasicBlock *parent, size_t &cur_idx) {
    BasicBlockInfo v_info(cur_idx, cur_idx);
    v_info.SetParent(parent);
    ++cur_idx;
    vertexes.push_back(v);
    bbs_info_.insert({v, std::move(v_info)});
    for(auto it = v->GetSuccBegin(), end = v->GetSuccEnd(); it != end; ++it) {
        if(!bbs_info_.contains(*it)) {
            DFS(*it, v, cur_idx);
        }
    }
}

void DomTree::RunStep2AndStep3() {
    for(auto &&vertex : vertexes) {
        bbs_info_.at(vertex).SetAncestor(nullptr);
        bbs_info_.at(vertex).SetLabel(vertex);
    }

    for(auto it = vertexes.rbegin(), end = --vertexes.rend(); it != end; ++it) {
        auto w = *it;
        for(auto pred_it = w->GetPredecBegin(), pred_end = w->GetPredecEnd(); pred_it != pred_end; ++pred_it) {
            const BasicBlock *u = Eval(*pred_it);
            if(bbs_info_.at(u).GetSemi() < bbs_info_.at(w).GetSemi()) {
                bbs_info_.at(w).SetSemi(bbs_info_.at(u).GetSemi());
            }
        }

        bbs_info_.at(vertexes.at(bbs_info_.at(w).GetSemi())).PushBackToBucket(w);
        Link(bbs_info_.at(w).GetParent(), w);
        auto w_parent_info = bbs_info_.at(bbs_info_.at(w).GetParent());
        while(!w_parent_info.IsBucketEmpty()) {
            const BasicBlock *last_v = w_parent_info.GetLastInBucket();
            const BasicBlock *u = Eval(last_v);
            const BasicBlock *dom = bbs_info_.at(u).GetSemi() < bbs_info_.at(last_v).GetSemi() ? u : bbs_info_.at(w).GetParent();
            bbs_info_.at(last_v).SetDom(dom);
            w_parent_info.PopBackFromBucket();
        }
    }
}

void DomTree::RunStep4() {
    for(auto it = ++vertexes.begin(), end = vertexes.end(); it != end; ++it) {
        const BasicBlock *w = *it;
        if(bbs_info_.at(w).GetDom() != vertexes.at(bbs_info_.at(w).GetSemi())) {
            const BasicBlock *dom = bbs_info_.at(bbs_info_.at(w).GetDom()).GetDom();
            bbs_info_.at(w).SetDom(dom);
        }
    }
    bbs_info_.at(graph_).SetDom(nullptr);
}

const BasicBlock *DomTree::Eval(const BasicBlock *v) {
    if(bbs_info_.at(v).GetAncestor() == nullptr) {
        return v;
    } else {
        Compress(v);
        return bbs_info_.at(v).GetLabel();
    }
}

void DomTree::Link(const BasicBlock *v_parent, const BasicBlock *v) {
    bbs_info_.at(v).SetAncestor(v_parent);
}

void DomTree::Compress(const BasicBlock *v) {
    if(bbs_info_.at(bbs_info_.at(v).GetAncestor()).GetAncestor() != nullptr) {
        Compress(bbs_info_.at(v).GetAncestor());
        if(bbs_info_.at(bbs_info_.at(bbs_info_.at(v).GetAncestor()).GetLabel()).GetSemi() <
            bbs_info_.at(bbs_info_.at(v).GetLabel()).GetSemi()) {
            const BasicBlock *v_label = bbs_info_.at(bbs_info_.at(v).GetAncestor()).GetLabel();
            bbs_info_.at(v).SetLabel(v_label);
        }
        const BasicBlock *v_ancestor = bbs_info_.at(bbs_info_.at(v).GetAncestor()).GetAncestor();
        bbs_info_.at(v).SetAncestor(v_ancestor);
    }
}

void DomTree::BuildDomTree() {
    for(auto &&v : vertexes) {
        BasicBlock *non_const_v = const_cast<BasicBlock*>(v);
        BasicBlock *idom = const_cast<BasicBlock*>(bbs_info_.at(v).GetDom());
        non_const_v->SetIdom(idom);
        if(idom != nullptr) {
            idom->PushBackDominatedV(non_const_v);
        }
    }
}
