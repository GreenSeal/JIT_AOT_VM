//
// Created by denist on 9/25/22.
//

#ifndef JIT_AOT_IN_VM_INSTRUCTIONS_H
#define JIT_AOT_IN_VM_INSTRUCTIONS_H
#include "ilist_nodes.h"

enum class inst_t {movi_u64, u32tou64, cmp_u64, ja, mul_u64, addi_u64, jmp, ret_u64};

class InstructionBase : public ilist_bidirectional_nodes<InstructionBase> {
public:
    InstructionBase(inst_t type) : type_(type) {}

    virtual ~InstructionBase() {}

    inst_t GetType() {
        return type_;
    }
private:
    inst_t type_;
};

class UnaryInstr : public InstructionBase {
public:
    UnaryInstr(inst_t type, std::unique_ptr<OperandBase>&& opnd1) : InstructionBase(type), opnd1_(opnd1) {}
    virtual ~UnaryInstr() {}
private:
    std::unique_ptr<OperandBase> opnd1_;
};

class BinaryInstr : public InstructionBase {
public:
    BinaryInstr(inst_t type, std::unique_ptr<OperandBase>&& opnd1, std::unique_ptr<OperandBase>&& opnd2) :
            UnaryInstr(type, opnd1), opnd2_(opnd2) {}
    virtual ~BinaryInstr() {}
private:
    std::unique_ptr<OperandBase> opnd2_;
};

class ThreearyInstr final : public InstructionBase {
public:
    ThreearyInstr(inst_t type, std::unique_ptr<OperandBase>&& opnd1, std::unique_ptr<OperandBase>&& opnd2,
                  std::unique_ptr<OperandBase>&& opnd3) : BinaryInstr(type, opnd1, opnd2), opnd3_(opnd3) {}
private:
    std::unique_ptr<InstructionBase> opnd3_;
};

#endif //JIT_AOT_IN_VM_INSTRUCTIONS_H
