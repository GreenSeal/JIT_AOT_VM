//
// Created by denist on 9/26/22.
//

#ifndef JIT_AOT_IN_VM_OPERANDS_H
#define JIT_AOT_IN_VM_OPERANDS_H
#include <concepts>
#include <vector>

#include "value.h"

enum class prim_type : uint8_t {UINT, INT, DOUBLE, NONE};

class Instruction;
class BasicBlock;

class Operand : public Value {
public:
    enum class opnd_t: uint8_t {simple, phi};

    const Instruction *GetUser() const {
        return user_;
    }

    Instruction *GetUser() {
        return user_;
    }

    void SetUser(Instruction *user) {
        user_ = user;
    }

    opnd_t GetOpndType() const {
        return opnd_type_;
    }

    virtual Operand *clone() const {
        return new Operand(*this);
    }

    virtual ~Operand() {}

protected:
    Operand(Instruction *user, opnd_t opnd_type) :
    Value(value_t::opnd), opnd_type_(opnd_type), user_(user) {}

    Operand(const Operand &other) = default;
    Operand &operator=(const Operand &other) = delete;

    Operand(Operand &&other) = delete;
    Operand &operator=(Operand &&other) = delete;

private:
    opnd_t opnd_type_;
    Instruction *user_;
};

class SimpleOperand : public Operand {
public:
    enum class simple_opnd_t{none, ireg, imm};

    prim_type GetPrimType() const {
        return p_type_;
    }

    uint8_t GetBitLenght() const {
        return bit_lenght_;
    }

    simple_opnd_t GetSimpleOpndType() const {
        return simpe_opnd_type_;
    }

    virtual SimpleOperand *clone() const override {
        return new SimpleOperand(*this);
    }

protected:
    SimpleOperand(Instruction *user, prim_type p_type, uint8_t bit_lenght,
                  simple_opnd_t simple_opnd_type = simple_opnd_t::none) :
    Operand(user, opnd_t::simple), p_type_(p_type), bit_lenght_(bit_lenght), simpe_opnd_type_(simple_opnd_type) {}

    SimpleOperand(const SimpleOperand &rhs) = default;

    prim_type p_type_;
    uint8_t bit_lenght_;
    simple_opnd_t simpe_opnd_type_;
};

class IReg : public SimpleOperand {
public:
    using idx_type = size_t;
    enum class reg_t{t, a, g, none};

    IReg(prim_type p_type, uint8_t bit_lenght, reg_t reg_type, idx_type idx, Instruction *user = nullptr) :
    SimpleOperand(user, p_type, bit_lenght, simple_opnd_t::ireg), reg_type_(reg_type), idx_(idx) {}

    idx_type GetRegIdx() const {
        return idx_;
    }

    void SetRegIdx(size_t idx) {
        idx_ = idx;
    }

    reg_t GetRegType() const {
        return reg_type_;
    }

    virtual IReg *clone() const {
        return new IReg(*this);
    }

    virtual ~IReg() = default;

private:
    IReg(const IReg &other) = default;

    reg_t reg_type_;
    idx_type idx_;
};

class Label final {
public:
    template <class Str>
    requires std::constructible_from<std::string, Str>
    Label(Str &&name, Instruction *inst = nullptr) : name_(name), labeled_inst_(inst){}

    const std::string_view GetName() const {
        return name_;
    }

    std::string_view GetName() {
        return name_;
    }

    Instruction *GetLabeledInst() {
        return labeled_inst_;
    }

    const Instruction *GetLabeledInst() const {
        return labeled_inst_;
    }

    void SetLabeledInst(Instruction *inst) {
        labeled_inst_ = inst;
    }

private:
    std::string name_;
    Instruction *labeled_inst_;
};



//TODO: вывести prim_type и bit_lenght из типа
template <class T>
requires std::integral<T> || std::floating_point<T>
class Immediate final : public SimpleOperand {
public:
    using type = T;

    Immediate(prim_type p_type, uint8_t bit_lenght, T value, Instruction *user = nullptr) :
    SimpleOperand(user, p_type, bit_lenght, simple_opnd_t::imm), value_(value) {}

    T GetValue() const {
        return value_;
    }

    void SetValue(T value) {
        value_ = value;
    }

    Immediate *clone() const override {
        return new Immediate<T>(*this);
    }

protected:
    Immediate(const Immediate<T> &rhs) = default;

private:
    T value_;
};

class PhiOperand final: public Operand {
public:
    PhiOperand(Instruction *user, BasicBlock *src, std::unique_ptr<SimpleOperand> &&opnd) :
    Operand(user, opnd_t::phi), opnd_{src, std::move(opnd)} {}

    PhiOperand(Instruction *user, BasicBlock *src, SimpleOperand *opnd) :
    Operand(user, opnd_t::phi), opnd_{src, std::unique_ptr<SimpleOperand>(opnd)} {}

    PhiOperand *clone() const override {
        return new PhiOperand(*this);
    }

    BasicBlock *GetSrcBB() {
        return opnd_.first;
    }

    const BasicBlock *GetSrcBB() const {
        return opnd_.first;
    }

    SimpleOperand *GetOperand() {
        return opnd_.second.get();
    }

    const SimpleOperand *GetOperand() const {
        return opnd_.second.get();
    }

    void SetOperand(SimpleOperand *opnd) {
        opnd_.second = std::unique_ptr<SimpleOperand>(opnd);
    }

protected:
    PhiOperand(const PhiOperand &rhs) : Operand(rhs), opnd_{rhs.opnd_.first, rhs.opnd_.second.get()->clone()} {}

private:
    std::pair<BasicBlock *, std::unique_ptr<SimpleOperand>> opnd_;
};



#endif //JIT_AOT_IN_VM_OPERANDS_H
