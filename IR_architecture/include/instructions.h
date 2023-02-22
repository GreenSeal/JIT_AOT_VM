//
// Created by denist on 9/25/22.
//

#ifndef JIT_AOT_IN_VM_INSTRUCTIONS_H
#define JIT_AOT_IN_VM_INSTRUCTIONS_H

#include <memory>

#include "user.h"
#include "ilist_nodes.h"

enum class inst_t {movi, u32tou64, cmp, ja, mul, add, jmp, ret, phi};
enum class class_t {none, unary, binary, jump, nary, phi};

class Instruction : public ilist_bidirectional_node<Instruction> {
public:
    Instruction(inst_t type, prim_type ptype) : type_(type), prim_type_(ptype) {}

    virtual Instruction *clone() const {
        return new Instruction(*this);
    }

    virtual ~Instruction() {}

    inst_t GetInstType() const {
        return type_;
    }

    static class_t GetClassType(inst_t inst_type) {
        switch(inst_type) {
            case inst_t::u32tou64: return class_t::unary;
            case inst_t::jmp: return class_t::jump;
            case inst_t::ja: return class_t::jump;
            case inst_t::ret: return class_t::unary;
            case inst_t::add: return class_t::binary;
            case inst_t::cmp: return class_t::binary;
            case inst_t::movi: return class_t::unary;
            case inst_t::mul: return class_t::binary;
            case inst_t::phi: return class_t::phi;
        }
    }

private:
    Instruction(const Instruction &other) : type_(other.type_), prim_type_(other.prim_type_) {};
    Instruction &operator=(const Instruction &other) = delete;

    Instruction(Instruction &&other) = delete;
    Instruction &&operator=(Instruction &&other) = delete;


protected:

    inst_t type_;
    prim_type prim_type_;
};

template <size_t opnds_num = std::numeric_limits<size_t>::max()>
class NarySimpleInstr : public Instruction, public User<SimpleOperand, opnds_num> {
public:
    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    NarySimpleInstr(inst_t type, std::unique_ptr<IReg> &&res, std::unique_ptr<Opnds> &&... opnd) :
    Instruction(type, res->GetPrimType()), User<SimpleOperand, opnds_num>(std::move(res), opnd...) {}

    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    NarySimpleInstr(inst_t type, IReg *res, Opnds *... opnd) :
            Instruction(type, res->GetPrimType()), User<SimpleOperand, opnds_num>(res, opnd...) {}

    virtual NarySimpleInstr *clone() const override {
        return new NarySimpleInstr(*this);
    }

private:
    NarySimpleInstr(const NarySimpleInstr &rhs) : Instruction(rhs.type_, rhs.prim_type_),
    User<SimpleOperand, opnds_num>(rhs) {};
    NarySimpleInstr &operator=(const NarySimpleInstr &) = delete;

    NarySimpleInstr(NarySimpleInstr &&) = delete;
    NarySimpleInstr &operator=(NarySimpleInstr &&) = delete;
};

class Jump : public Instruction {
public:

    template<class Str>
    requires std::same_as<Str, std::string> || std::convertible_to<Str, const char *>
    Jump(Str &&label_name, inst_t jmp_type = inst_t::jmp) : Instruction(jmp_type, prim_type::NONE),
    label_(label_name) {}

    std::string_view GetLabelName() {
        return label_.GetName();
    }

private:
    Jump(const Jump &rhs) : Instruction(rhs.type_, rhs.prim_type_), label_(rhs.label_) {}

protected:
    Label label_;
};

class PhiInst final : public Instruction,
        protected User<PhiOperand, std::numeric_limits<size_t>::max()> {
    using PhiOpnd = OperandType;

public:
    template <class ...Args>
    requires IsPackSameType<PhiOpnd, Args...>
    PhiInst(std::unique_ptr<IReg> res, std::unique_ptr<Args> && ...args) : Instruction(inst_t::phi, res->GetPrimType()),
    User<PhiOpnd, std::numeric_limits<size_t>::max()>(std::move(res), std::move(args)...) {}

    template <class ...Args>
    requires IsPackSameType<PhiOpnd, Args...>
    PhiInst(IReg *res, Args *...args) :
    Instruction(inst_t::phi, res->GetPrimType()),
    User<PhiOpnd, std::numeric_limits<size_t>::max()>(std::unique_ptr<IReg>(res), std::unique_ptr<Args>(args)...) {}

    const std::string_view GetLabelAt(size_t idx) const {
        return GetOpndAt(idx)->GetLabel();
    }

    std::string_view GetLabelAt(size_t idx) {
        return GetOpndAt(idx)->GetLabel();
    }

    const IReg *GetRes() const {
        return GetResReg();
    }

    IReg *GetRes() {
        return GetResReg();
    }

    const SimpleOperand *GetOperandAt(size_t idx) const {
        return GetOpndAt(idx)->GetOperand();
    }

    SimpleOperand *GetOperandAt(size_t idx) {
        return GetOpndAt(idx)->GetOperand();
    }

    virtual PhiInst *clone() const override {
        return new PhiInst(*this);
    }

private:
    PhiInst(const PhiInst &rhs) : Instruction(rhs.type_, rhs.prim_type_),
    User<PhiOpnd , std::numeric_limits<size_t>::max()>(rhs) {}

};


#endif //JIT_AOT_IN_VM_INSTRUCTIONS_H
