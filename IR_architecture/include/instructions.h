//
// Created by denist on 9/25/22.
//

#ifndef JIT_AOT_IN_VM_INSTRUCTIONS_H
#define JIT_AOT_IN_VM_INSTRUCTIONS_H

#include <memory>

#include "user.h"
#include "ilist_nodes.h"


enum class inst_t {movi, u32tou64, cmp, ja, mul, add, jmp, ret, phi, static_call};
enum class class_t {unary, binary, jump, nary, phi};

class BasicBlock;
class IRFunction;

// idea: make virtual methods for accessing res reg and opnds
class Instruction : public ilist_bidirectional_node<Instruction> {
public:
    Instruction(inst_t type, prim_type ptype, bool is_jump_dest = false)
    : is_jump_dest_(is_jump_dest), parent_bb_(nullptr), type_(type), prim_type_(ptype) {}

    virtual Instruction *clone() const {
        return new Instruction(*this);
    }

    virtual ~Instruction() = default;

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
            case inst_t::static_call: return class_t::nary;
            default: throw std::invalid_argument("Unsupported instruction in GetClassType");
        }
    }

    BasicBlock *GetParentBB() {
        return parent_bb_;
    }

    const BasicBlock *GetParentBB() const {
        return parent_bb_;
    }

    void SetParentBB(BasicBlock *bb) {
        parent_bb_ = bb;
    }

    bool IsJumpDest() const {
        return is_jump_dest_;
    }

    void SetJumpDest(bool flag) {
        is_jump_dest_ = flag;
    }

    prim_type GetPrimType() const {
        return prim_type_;
    }

    virtual IReg *GetResReg() {
        return nullptr;
    }

    virtual const IReg *GetResReg() const {
        return nullptr;
    }

    virtual Operand *GetOpnd(size_t) {
        return nullptr;
    }

    virtual const Operand *GetOpnd(size_t) const {
        return nullptr;
    }

    virtual void SetOpnd(size_t, Operand *) {
        return;
    }

    virtual size_t OpndsSize() const {
        return 0;
    }

protected:
    Instruction(const Instruction &other) = default;
    Instruction &operator=(const Instruction &other) = delete;

    Instruction(Instruction &&other) = delete;
    Instruction &&operator=(Instruction &&other) = delete;


protected:

    bool is_jump_dest_;
    BasicBlock *parent_bb_;
    inst_t type_;
    prim_type prim_type_;
};

inline bool IsInstWithSimpleOpnds(Instruction *inst) {
    switch(Instruction::GetClassType(inst->GetInstType())) {
        case class_t::unary:
        case class_t::binary:
        case class_t::nary:
            return true;
        case class_t::phi:
        case class_t::jump:
        default:
            return false;
    }
}

template <class OpndType, size_t opnd_num = std::numeric_limits<size_t>::max()>
class InstrWithOpnd : public Instruction, protected User<OpndType, opnd_num> {
protected:
    using User<OpndType, opnd_num>::GetResultReg;
    using User<OpndType, opnd_num>::GetOperandsSize;
    using User<OpndType, opnd_num>::SetOperand;
    using User<OpndType, opnd_num>::GetOperand;

    template <class ...Opnds>
    InstrWithOpnd(inst_t type, bool is_jump_dest, std::unique_ptr<IReg> &&res, std::unique_ptr<Opnds> && ...opnd) :
            Instruction(type, res.get()? res->GetPrimType() : prim_type::NONE, is_jump_dest),
            User<OpndType, opnd_num>(std::move(res), opnd...) {}

    template <class ...Opnds>
    InstrWithOpnd(inst_t type, bool is_jump_dest, IReg *res, Opnds * ...opnd) :
            Instruction(type, res? res->GetPrimType() : prim_type::NONE, is_jump_dest),
            User<OpndType, opnd_num>(res, opnd...) {};

public:
    virtual IReg *GetResReg() override {
        return GetResultReg();
    }

    virtual const IReg *GetResReg() const override {
        return GetResultReg();
    }

    virtual OpndType *GetOpnd(size_t idx) override {
        return GetOperand(idx);
    }

    virtual const OpndType *GetOpnd(size_t idx) const override {
        return GetOperand(idx);
    }

    virtual void SetOpnd(size_t idx, Operand *opnd) override {
        return SetOperand(idx, static_cast<OpndType*>(opnd));
    }

    virtual size_t OpndsSize() const override {
        return GetOperandsSize();
    }

    virtual ~InstrWithOpnd() = default;
};

template <size_t opnds_num = std::numeric_limits<size_t>::max()>
class NarySimpleInstr : public InstrWithOpnd<SimpleOperand, opnds_num> {
public:
    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    NarySimpleInstr(inst_t type, bool is_jump_dest, std::unique_ptr<IReg> &&res, std::unique_ptr<Opnds> &&... opnd) :
    InstrWithOpnd<SimpleOperand, opnds_num>(type, is_jump_dest, res, opnd...) {}

    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    NarySimpleInstr(inst_t type, bool is_jump_dest, IReg *res, Opnds *... opnd) :
    InstrWithOpnd<SimpleOperand, opnds_num>(type, is_jump_dest, res, opnd...) {}

    virtual NarySimpleInstr *clone() const override {
        return new NarySimpleInstr(*this);
    }

    virtual ~NarySimpleInstr() = default;

protected:
    NarySimpleInstr(const NarySimpleInstr &rhs) = default;
    NarySimpleInstr &operator=(const NarySimpleInstr &) = delete;

    NarySimpleInstr(NarySimpleInstr &&) = delete;
    NarySimpleInstr &operator=(NarySimpleInstr &&) = delete;
};

class Jump : public InstrWithOpnd<LabelOpnd, 1> {
public:
    template<class Str>
    requires std::same_as<Str, std::string> || std::convertible_to<Str, const char *>
    Jump(Str &&label_name, bool is_jump_dest = false, Instruction *jmp_dest = nullptr, inst_t jmp_type = inst_t::jmp) :
    InstrWithOpnd<LabelOpnd, 1>(jmp_type, is_jump_dest, nullptr,
                                new LabelOpnd(std::move(label_name), jmp_dest)) {}

    LabelOpnd *GetOpnd(size_t) override {
        return GetOperand(0);
    }

    const LabelOpnd *GetOpnd(size_t) const override {
        return GetOperand(0);
    }

    void SetOpnd(size_t, Operand *opnd) override {
        SetOperand(0, static_cast<LabelOpnd*>(opnd));
    }

    virtual Jump *clone() const override {
        return new Jump(*this);
    }

    virtual ~Jump() = default;

protected:
    Jump(const Jump &rhs) = default;
};

class PhiInst final : public InstrWithOpnd<PhiOperand> {
    using PhiOpnd = OperandType;

public:
    template <class ...Args>
    requires IsPackSameType<PhiOpnd, Args...>
    PhiInst(std::unique_ptr<IReg> res, std::unique_ptr<Args> && ...args) :
    InstrWithOpnd<PhiOperand>(inst_t::phi, true,
                              std::move(res), std::move(args)...) {}

    template <class ...Args>
    requires IsPackSameType<PhiOpnd, Args...>
    PhiInst(IReg *res, Args *...args) :
    InstrWithOpnd<PhiOperand>(inst_t::phi, true, res, args...) {}

    const BasicBlock *GetSrcBBAt(size_t idx) const {
        return GetOperandAt(idx)->GetSrcBB();
    }

    BasicBlock *GetSrcBBAt(size_t idx) {
        return GetOperandAt(idx)->GetSrcBB();
    }

    void PushBackOperand(BasicBlock *bb, SimpleOperand *opnd) {
        opnds_.emplace_back(std::make_unique<PhiOpnd>(opnd, bb, this));
    }

    virtual PhiInst *clone() const override {
        return new PhiInst(*this);
    }

protected:
    PhiInst(const PhiInst &rhs) = default;

};

class CallInstr : public InstrWithOpnd<SimpleOperand> {
public:

    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    CallInstr(bool is_jump_dest, IRFunction *func, std::unique_ptr<IReg> &&res,
    std::unique_ptr<Opnds> && ...opnd) :
    InstrWithOpnd<SimpleOperand>(inst_t::static_call, is_jump_dest,
                                 std::move(res), std::move(opnd)...), callee_func_(func) {}


    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    CallInstr(bool is_jump_dest, IRFunction *func, IReg *res, Opnds *... opnd) :
            InstrWithOpnd<SimpleOperand>(inst_t::static_call, is_jump_dest, res, opnd...),
            callee_func_(func) {}

    IRFunction *GetFunc() {
        return callee_func_;
    }

    const IRFunction *GetFunc() const {
        return callee_func_;
    }

    virtual CallInstr *clone() const override {
        return new CallInstr(*this);
    }

    virtual ~CallInstr() = default;

protected:
    CallInstr(const CallInstr &rhs) = default;
    CallInstr &operator=(const CallInstr *rhs) = delete;

    CallInstr(CallInstr &&rhs) = delete;
    CallInstr &operator=(CallInstr &&rhs) = delete;

    IRFunction *callee_func_;
};


#endif //JIT_AOT_IN_VM_INSTRUCTIONS_H
