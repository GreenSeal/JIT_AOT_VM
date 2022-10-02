#include "ilist_nodes.h"

#ifndef VM_IR_GRAPH_H
#define VM_IR_GRAPH_H

class IRFunction;

class RuntimeInfo {};
class BackendInfo {};
class OptimisationsInfo {};

class IRGraphStorage : public RuntimeInfo, BackendInfo, OptimisationsInfo {

protected:
    IRFunction *func_;
    BasicBlock *root_;
};

class IRGraphBuilder : public IRGraphStorage {
public:
    using sztype = size_t;

    void CreateAndInsertBBBack(InstructionBase *start_instr, const std::string &label = "") {
        auto *new_bb = new BasicBlock(this, label, start_instr);

        root_->SetNext(new_bb);
        new_bb->SetPrev(root_);
    }

    void CreateFunction() {
        
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
private:
    BasicBlock *inserter_cursor_;
};

#endif // VM_IR_GRAPH_H