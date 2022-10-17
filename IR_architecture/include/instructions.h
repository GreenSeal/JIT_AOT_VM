//
// Created by denist on 9/25/22.
//

#ifndef JIT_AOT_IN_VM_INSTRUCTIONS_H
#define JIT_AOT_IN_VM_INSTRUCTIONS_H

#include <memory>

#include "operands.h"
#include "ilist_nodes.h"

enum class inst_t {movi, u32tou64, cmp, ja, mul, addi, jmp, ret};
enum class class_t {base, unary, binary, ternary};

class InstructionBase : public ilist_bidirectional_node<InstructionBase> {
public:
    InstructionBase(class_t class_type, inst_t type, prim_type ptype) : class_type_(class_type), type_(type), prim_type_(ptype) {}
    InstructionBase(const InstructionBase &rhs) : class_type_(rhs.class_type_), type_(rhs.type_) {
        SetNext(nullptr);
        SetPrev(nullptr);
    }

    virtual ~InstructionBase() {}

    virtual InstructionBase *clone() const {
        return new InstructionBase(*this);
    }

    inst_t GetType() const {
        return type_;
    }

    class_t GetClassType() const {
        return class_type_;
    }

protected:

    class_t class_type_;
    inst_t type_;
    prim_type prim_type_;
};

class UnaryInstr : public InstructionBase {
public:
    UnaryInstr(std::unique_ptr<OperandBase>&& opnd1, inst_t type, prim_type ptype, class_t class_type = class_t::unary) :
    InstructionBase(class_type, type, ptype), opnd1_(std::move(opnd1)) {}

    UnaryInstr(OperandBase *opnd1, inst_t type, prim_type ptype, class_t class_type = class_t::unary) :
            InstructionBase(class_type, type, ptype), opnd1_(opnd1) {}

    UnaryInstr(const UnaryInstr &rhs) : InstructionBase(rhs), opnd1_(rhs.GetOpnd(0)->clone()) {}

    virtual ~UnaryInstr() {}

    virtual UnaryInstr *clone() const override {
        return new UnaryInstr(*this);
    }

    virtual const OperandBase *GetOpnd(size_t) const {
        return opnd1_.get();
    }

protected:
    std::unique_ptr<OperandBase> opnd1_;
};

class BinaryInstr : public UnaryInstr {
public:
    BinaryInstr(std::unique_ptr<OperandBase>&& opnd1, std::unique_ptr<OperandBase>&& opnd2, inst_t type,
                prim_type ptype, class_t class_type) :
            UnaryInstr(std::move(opnd1), type, ptype, class_type), opnd2_(std::move(opnd2)) {}

    BinaryInstr(OperandBase *opnd1, OperandBase *opnd2, inst_t type, prim_type ptype, class_t class_type = class_t::binary) :
    UnaryInstr(opnd1, type, ptype, class_type), opnd2_(opnd2) {}

    BinaryInstr(const BinaryInstr &rhs) : UnaryInstr(rhs), opnd2_(rhs.GetOpnd(1)->clone()) {};

    virtual ~BinaryInstr() {}

    virtual BinaryInstr *clone() const override {
        return new BinaryInstr(*this);
    }

    virtual const OperandBase* GetOpnd(size_t idx) const override{
        if(idx == 0) {
            return opnd1_.get();
        } else {
            return opnd2_.get();
        }
    }
protected:
    std::unique_ptr<OperandBase> opnd2_;
};

class TernaryInstr final : public BinaryInstr {
public:
    TernaryInstr(std::unique_ptr<OperandBase>&& opnd1, std::unique_ptr<OperandBase>&& opnd2,
                  std::unique_ptr<OperandBase>&& opnd3, inst_t type, prim_type ptype) :
                  BinaryInstr(std::move(opnd1), std::move(opnd2), type, ptype, class_t::ternary), opnd3_(std::move(opnd3)) {}

    TernaryInstr(OperandBase *opnd1, OperandBase *opnd2, OperandBase *opnd3, inst_t type, prim_type ptype) :
            BinaryInstr(opnd1, opnd2, type, ptype, class_t::ternary), opnd3_(opnd3) {}

    TernaryInstr(const TernaryInstr &rhs) : BinaryInstr(rhs), opnd3_(rhs.GetOpnd(2)->clone()) {};

    TernaryInstr *clone() const override {
        return new TernaryInstr(*this);
    }

    virtual const OperandBase *GetOpnd(size_t idx) const override {
        if(idx == 0) {
            return opnd1_.get();
        } else if(idx == 1) {
            return opnd2_.get();
        } else {
            return opnd3_.get();
        }
    }
private:
    std::unique_ptr<OperandBase> opnd3_;
};

#endif //JIT_AOT_IN_VM_INSTRUCTIONS_H
