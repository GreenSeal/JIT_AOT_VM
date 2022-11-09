

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

    IRGraph(BasicBlock *root = nullptr, IRFunction *func = nullptr);
    IRGraph(const IRGraph &rhs);

    // void CreateAndInsertBBBack(InstructionBase *start_instr, const std::string &label = "");

    std::unique_ptr<BinaryInstr> CreateMoviU64(IReg::reg_t reg_type, IReg::idx_type reg_idx, uint64_t imm_value) {
        return std::unique_ptr<BinaryInstr>(new BinaryInstr(new IReg(reg_type, reg_idx, prim_type::u64),
                                                            new UInt32Const(imm_value),
                                                            inst_t::movi, prim_type::u64, class_t::binary));
    }

    std::unique_ptr<BinaryInstr> CreateU32ToU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                    IReg::reg_t reg_type2, IReg::idx_type reg_idx2) {
        return std::unique_ptr<BinaryInstr>(new BinaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                            new IReg(reg_type2, reg_idx2, prim_type::u32),
                                                            inst_t::u32tou64, prim_type::none, class_t::binary));
    }

    std::unique_ptr<BinaryInstr> CreateCmpU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                  IReg::reg_t reg_type2, IReg::idx_type reg_idx2) {
        return std::unique_ptr<BinaryInstr>(new BinaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                            new IReg(reg_type2, reg_idx2, prim_type::u64),
                                                            inst_t::cmp, prim_type::u64, class_t::binary));
    }

    std::unique_ptr<UnaryInstr> CreateJa(const std::string &label_name) {
        return std::unique_ptr<UnaryInstr>(new UnaryInstr(new Label(label_name),
                                                          inst_t::ja, prim_type::none, class_t::unary));
    }

    std::unique_ptr<TernaryInstr> CreateMulU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                  IReg::reg_t reg_type2, IReg::idx_type reg_idx2,
                                                  IReg::reg_t reg_type3, IReg::idx_type reg_idx3) {
        return std::unique_ptr<TernaryInstr>(new TernaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                              new IReg(reg_type2, reg_idx2, prim_type::u64),
                                                          new IReg(reg_type3, reg_idx3, prim_type::u64), inst_t::mul, prim_type::u64));
    }

    std::unique_ptr<TernaryInstr> CreateAddiU64(IReg::reg_t reg_type1, IReg::idx_type reg_idx1,
                                                   IReg::reg_t reg_type2, IReg::idx_type reg_idx2,
                                                   uint32_t imm_value) {
        return std::unique_ptr<TernaryInstr>(new TernaryInstr(new IReg(reg_type1, reg_idx1, prim_type::u64),
                                                              new IReg(reg_type2, reg_idx2, prim_type::u64),
                                                              new UInt32Const(imm_value), inst_t::addi, prim_type::u64));
    }

    std::unique_ptr<UnaryInstr> CreateJmp(const std::string &label_name) {
        return std::unique_ptr<UnaryInstr>(new UnaryInstr(new Label(label_name), inst_t::jmp, prim_type::none, class_t::unary));
    }

    std::unique_ptr<UnaryInstr> CreateRetU64(IReg::reg_t reg_type, IReg::idx_type reg_idx) {
        return std::unique_ptr<UnaryInstr>(new UnaryInstr(new IReg(reg_type, reg_idx, prim_type::u64),
                                                          inst_t::ret, prim_type::u64, class_t::unary));
    }

    bool IsBBsConnected(BasicBlock *bb, BasicBlock *bb_succ) const;
    void AddBBToBegin(BasicBlock *bb);

    bool IsBBInGraph(BasicBlock *bb) {
        return bb_set_.contains(bb);
    }

    void AddBBInGraph(BasicBlock *bb) {
        bb_set_.insert(bb);
    }

    void AddEdge(BasicBlock *from, BasicBlock *to) {
        if(!IsBBInGraph(from) || !IsBBInGraph(to)) {
            throw std::invalid_argument("Can't add an edge between vertices that don't exist in the graph");
        }

        if(from->SuccSize() == 2) {
            throw std::invalid_argument("Trying to add third successor at AddEdge method");
        }

        if(from->SuccSize() == 1 && *(from->GetSuccBegin()) == to) {
            throw std::invalid_argument("Trying to add existing edge at AddEdge method");
        }

        from->AddSucc(to);
        to->AddPredec(from);
    }

    void RemoveEdge(BasicBlock *from, BasicBlock *to) {
        if(!IsBBInGraph(from) || !IsBBInGraph(to)) {
            throw std::invalid_argument("Can't remove an edge between vertices that don't exist in the graph");
        }

        if(from->SuccSize() == 0) {
            throw std::invalid_argument("Trying to remove non-existing edge at RemoveEdge method");
        }


        for(auto it = from->GetSuccBegin(), end = from->GetSuccEnd(); it != end; ++it) {
            if(*it == to) {
                from->RemoveSucc(*it);
                to->RemovePredec(from);
                return;
            }
        }

        throw std::invalid_argument("Trying to remove non-existing edge at RemoveEdge method");
    }

    BasicBlock *GetRoot() {
        return root_;
    }

    const BasicBlock *GetRoot() const {
        return root_;
    }

protected:
    IRFunction *func_;
    std::unordered_set<BasicBlock*> bb_set_;
    BasicBlock *root_;
};

#endif // VM_IR_GRAPH_H