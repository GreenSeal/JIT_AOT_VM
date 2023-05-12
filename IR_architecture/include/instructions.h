//
// Created by denist on 9/25/22.
//

#ifndef JIT_AOT_IN_VM_INSTRUCTIONS_H
#define JIT_AOT_IN_VM_INSTRUCTIONS_H

#include <memory>

#include "user.h"
#include "ilist_nodes.h"


enum class inst_t {movi, u32tou64, cmp, ja, mul, add, jmp, ret, phi, call};
enum class class_t {none, unary, binary, jump, nary, phi};

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
            case inst_t::call: return class_t::nary;
            default: return class_t::none;
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

private:
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

template <size_t opnds_num = std::numeric_limits<size_t>::max()>
class NarySimpleInstr : public Instruction, public User<SimpleOperand, opnds_num> {
public:
    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    NarySimpleInstr(inst_t type, bool is_jump_dest, std::unique_ptr<IReg> &&res, std::unique_ptr<Opnds> &&... opnd) :
    Instruction(type, res.get()? res->GetPrimType() : prim_type::NONE, is_jump_dest), User<SimpleOperand, opnds_num>(std::move(res), opnd...) {}

    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    NarySimpleInstr(inst_t type, bool is_jump_dest, IReg *res, Opnds *... opnd) :
    Instruction(type, res? res->GetPrimType() : prim_type::NONE, is_jump_dest), User<SimpleOperand, opnds_num>(res, opnd...) {}

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
    Jump(Str &&label_name, Instruction *jmp_dest= nullptr, inst_t jmp_type = inst_t::jmp) : Instruction(jmp_type, prim_type::NONE),
    label_(label_name, jmp_dest) {}

    std::string_view GetLabelName() {
        return label_.GetName();
    }

    Instruction *GetInstToJump() {
        return label_.GetLabeledInst();
    }

    const Instruction *GetInstToJump() const {
        return label_.GetLabeledInst();
    }

    void SetInstToJump(Instruction *dest) {
        label_.SetLabeledInst(dest);
    }

private:
    Jump(const Jump &rhs) : Instruction(rhs.type_, rhs.prim_type_), label_(rhs.label_) {}

protected:
    Label label_;
};

class PhiInst final : public Instruction,
        protected User<PhiOperand> {
    using PhiOpnd = OperandType;

public:
    template <class ...Args>
    requires IsPackSameType<PhiOpnd, Args...>
    PhiInst(std::unique_ptr<IReg> res, std::unique_ptr<Args> && ...args) : Instruction(inst_t::phi, res->GetPrimType()),
    User<PhiOpnd>(std::move(res), std::move(args)...) {}

    template <class ...Args>
    requires IsPackSameType<PhiOpnd, Args...>
    PhiInst(IReg *res, Args *...args) :
    Instruction(inst_t::phi, res->GetPrimType(), true),
    User<PhiOpnd>(std::unique_ptr<IReg>(res), std::unique_ptr<Args>(args)...) {}

    const BasicBlock *GetSrcBBAt(size_t idx) const {
        return GetOpndAt(idx)->GetSrcBB();
    }

    BasicBlock *GetSrcBBAt(size_t idx) {
        return GetOpndAt(idx)->GetSrcBB();
    }

    const IReg *GetRes() const {
        return GetResReg();
    }

    IReg *GetRes() {
        return GetResReg();
    }

    size_t GetOperandsSize() const {
        return GetOpndsSize();
    }

    const SimpleOperand *GetOperandAt(size_t idx) const {
        return GetOpndAt(idx)->GetOperand();
    }

    SimpleOperand *GetOperandAt(size_t idx) {
        return GetOpndAt(idx)->GetOperand();
    }

    void SetOperandAt(size_t idx, SimpleOperand *opnd) {
        GetOpndAt(idx)->SetOperand(opnd);
    }

    void PushBackOperand(BasicBlock *bb, SimpleOperand *opnd) {
        opnds_.emplace_back(std::make_unique<PhiOpnd>(this, bb, opnd));
    }

    virtual PhiInst *clone() const override {
        return new PhiInst(*this);
    }

private:
    PhiInst(const PhiInst &rhs) : Instruction(rhs.type_, rhs.prim_type_),
    User<PhiOpnd , std::numeric_limits<size_t>::max()>(rhs) {}

};

class CallInstr : public Instruction, public User<SimpleOperand> {
public:

    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    CallInstr(bool is_jump_dest, IRFunction *func, std::unique_ptr<IReg> &&res,
    std::unique_ptr<Opnds> && ...opnd) :
    Instruction(inst_t::call, res->GetPrimType(), is_jump_dest),
    User<SimpleOperand>(std::move(res), std::move(opnd)...), callee_func_(func) {}


    template<class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    CallInstr(bool is_jump_dest, IRFunction *func, IReg *res, Opnds *... opnd) :
    Instruction(inst_t::call, res->GetPrimType(), is_jump_dest),
    User<SimpleOperand>(std::unique_ptr<IReg>(res), std::unique_ptr<SimpleOperand>(opnd)...),
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

protected:
    CallInstr(const CallInstr &rhs) :
    Instruction(inst_t::call, rhs.prim_type_, rhs.is_jump_dest_),
    User<SimpleOperand>(rhs), callee_func_(rhs.callee_func_) {}
    CallInstr &operator=(const CallInstr *rhs) = delete;

    CallInstr(CallInstr &&rhs) = delete;
    CallInstr &operator=(CallInstr &&rhs) = delete;

    IRFunction *callee_func_;
};


#endif //JIT_AOT_IN_VM_INSTRUCTIONS_H
