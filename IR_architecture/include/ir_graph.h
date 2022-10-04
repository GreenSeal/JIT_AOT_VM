

#ifndef VM_IR_GRAPH_H
#define VM_IR_GRAPH_H

#include <list>
#include <unordered_map>
#include <queue>
#include <unordered_set>

#include "ilist_nodes.h"
#include "basic_block.h"

class IRFunction;

class RuntimeInfo {};
class BackendInfo {};
class OptimisationsInfo {};


class IRGraph : public RuntimeInfo, BackendInfo, OptimisationsInfo {
public:
    using sztype = size_t;

    IRGraph(BasicBlock *root, IRFunction *func = nullptr) : func_(func) {
        if(root != nullptr) {

            // first we go around all the vertices of the graph using BFS algorythm and copy them
            // ==================================================================================
            std::unordered_map<BasicBlock*, BasicBlock*> old_to_new_map;

            std::unordered_set<BasicBlock*> explored_old_bbs;
            std::queue<BasicBlock*> bbs_to_copy;

            bbs_to_copy.push(root);
            explored_old_bbs.insert(root);

            while(!bbs_to_copy.empty()) {
                BasicBlock *old_bb = bbs_to_copy.front();
                bbs_to_copy.pop();

                BasicBlock *new_bb = old_bb->clone();
                old_to_new_map[old_bb] = new_bb;

                adjacency_list_[new_bb] = std::list<BasicBlock*>();

                for(size_t idx = 0; idx < old_bb->SuccSize(); ++idx) {
                    if(!explored_old_bbs.contains(old_bb->GetNext(idx))) {
                        bbs_to_copy.push(old_bb->GetNext(idx));
                        explored_old_bbs.insert(old_bb->GetNext(idx));
                    }
                }
            }

            // ==================================================================================

            // then fill adjacency list using again BFS
            // ==================================================================================
            explored_old_bbs.clear();

            bbs_to_copy.push(root);
            explored_old_bbs.insert(root);

            while(!bbs_to_copy.empty()) {
                BasicBlock *old_bb = bbs_to_copy.front();
                bbs_to_copy.pop();

                BasicBlock *new_bb = old_to_new_map.at(old_bb);
                for(size_t idx = 0; idx < old_bb->PredecSize(); ++idx) {
                    BasicBlock *old_pred = old_bb->GetPrev(idx);
                    BasicBlock *new_pred = old_to_new_map.at(old_pred);
                    new_bb->PushBackPredec(new_pred);
                }

                for(size_t idx = 0; idx < old_bb->SuccSize(); ++idx) {
                    BasicBlock *old_succ = old_bb->GetNext(idx);
                    BasicBlock *new_succ = old_to_new_map.at(old_succ);
                    new_bb->PushBackSucc(new_succ);
                    adjacency_list_.at(new_bb).push_back(new_succ);

                    if(!explored_old_bbs.contains(old_succ)) {
                        bbs_to_copy.push(old_succ);
                        explored_old_bbs.insert(old_succ);
                    }
                }
            }
            // ==================================================================================
            root_ = old_to_new_map.at(root);
            inserter_cursor_ = root_;
        }
    }

    void CreateAndInsertBBBack(InstructionBase *start_instr, const std::string &label = "") {
        auto *new_bb = new BasicBlock(this, label, start_instr);

        root_->PushBackSucc(new_bb);
        new_bb->PushBackPredec(root_);
    }

    std::unique_ptr<BinaryInstr> CreateMoviU64(IReg::reg_t reg_type, IReg::idx_type reg_idx, uint64_t imm_value) {
        return std::unique_ptr<BinaryInstr>(new BinaryInstr(new IReg(reg_type, reg_idx, prim_type::u64),
                                                            new UInt32Const(imm_value),
                                                            inst_t::movi_u64, class_t::binary));
    }

    std::unique_ptr<BinaryInstr> CreateU32ToU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                    IReg::reg_t reg_type2, IReg::idx_type reg_idx2) {
        return std::unique_ptr<BinaryInstr>(new BinaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                            new IReg(reg_type2, reg_idx2, prim_type::u32),
                                                            inst_t::u32tou64, class_t::binary));
    }

    std::unique_ptr<BinaryInstr> CreateCmpU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                  IReg::reg_t reg_type2, IReg::idx_type reg_idx2) {
        return std::unique_ptr<BinaryInstr>(new BinaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                            new IReg(reg_type2, reg_idx2, prim_type::u64),
                                                            inst_t::cmp_u64, class_t::binary));
    }

    std::unique_ptr<UnaryInstr> CreateJa(const std::string &label_name) {
        return std::unique_ptr<UnaryInstr>(new UnaryInstr(new Label(label_name),
                                                          inst_t::ja, class_t::unary));
    }

    std::unique_ptr<TernaryInstr> CreateMulU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                  IReg::reg_t reg_type2, IReg::idx_type reg_idx2,
                                                  IReg::reg_t reg_type3, IReg::idx_type reg_idx3) {
        return std::unique_ptr<TernaryInstr>(new TernaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                              new IReg(reg_type2, reg_idx2, prim_type::u64),
                                                          new IReg(reg_type3, reg_idx3, prim_type::u64), inst_t::mul_u64));
    }

    std::unique_ptr<TernaryInstr> CreateAddiU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                   IReg::reg_t reg_type2, IReg::idx_type reg_idx2,
                                                   uint32_t imm_value) {
        return std::unique_ptr<TernaryInstr>(new TernaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                              new IReg(reg_type2, reg_idx2, prim_type::u64),
                                                              new UInt32Const(imm_value), inst_t::addi_u64));
    }

    std::unique_ptr<UnaryInstr> CreateJmp(const std::string &label_name) {
        return std::unique_ptr<UnaryInstr>(new UnaryInstr(new Label(label_name), inst_t::jmp, class_t::unary));
    }

    std::unique_ptr<UnaryInstr> CreateRetU64(IReg::reg_t reg_type, IReg::idx_type reg_idx) {
        return std::unique_ptr<UnaryInstr>(new UnaryInstr(new IReg(reg_type, reg_idx, prim_type::u64),
                                                          inst_t::ret_u64, class_t::unary));
    }

    bool IsBBsConnected(BasicBlock *bb, BasicBlock *bb_succ) const {
        for(auto it = adjacency_list_.at(bb).begin(), end = adjacency_list_.at(bb).end(); it != end; ++it) {
            if(*it == bb_succ)
                return true;
        }

        return false;
    }

    bool IsBBInGraph(BasicBlock *bb) {
        return adjacency_list_.contains(bb);
    }

    void PushSuccForCurBB(const BasicBlock *bb) {

    }

    void PushPredecForCurBB(const BasicBlock *bb) {

    }

    void InsertInstAtEndCurBB(const InstructionBase *inst) {

    }

    void InsertInstArBeginCurBB(const InstructionBase *inst) {

    }

    BasicBlock *MoveInserterNext(size_t idx) {
        inserter_cursor_ = inserter_cursor_->GetNext(idx);
        return inserter_cursor_;
    }

    BasicBlock *MoveInserterPrev(size_t idx) {
        inserter_cursor_ = inserter_cursor_->GetPrev(idx);
        return inserter_cursor_;
    }

    BasicBlock *GetInsertCursor() const {
        return inserter_cursor_;
    }

    void ResetInsertCursor() {
        inserter_cursor_ = root_;
    }

protected:
    IRFunction *func_;
    std::unordered_map<BasicBlock*, std::list<BasicBlock*>> adjacency_list_;
    BasicBlock *root_;
    BasicBlock *inserter_cursor_;
};

#endif // VM_IR_GRAPH_H