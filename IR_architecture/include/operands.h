//
// Created by denist on 9/26/22.
//

#ifndef JIT_AOT_IN_VM_OPERANDS_H
#define JIT_AOT_IN_VM_OPERANDS_H
#include <concepts>
#include <vector>

#include "value.h"

enum class prim_type : uint8_t {UINT, INT, DOUBLE, NONE};

class Operand : public Value {
public:
    enum class opnd_t: uint8_t {simple, phi};

    const Value *GetUser() const {
        return user_;
    }

    Value *GetUser() {
        return user_;
    }

    void SetUser(Value *user) {
        user_ = user;
    }

    opnd_t GetOpndType() const {
        return opnd_type_;
    }

    virtual ~Operand() {}

protected:
    Operand(Value *user, opnd_t opnd_type) :
    Value(value_t::opnd), opnd_type_(opnd_type), user_(user) {}

    Operand(const Operand &other) = delete;
    Operand &operator=(const Operand &other) = delete;

    Operand(Operand &&other) = delete;
    Operand &operator=(Operand &&other) = delete;

private:
    opnd_t opnd_type_;
    Value *user_;
};

class SimpleOperand : public Operand {
public:
    enum class simple_opnd_t{ireg, imm};

    prim_type GetPrimType() const {
        return p_type_;
    }

    uint8_t GetBitLenght() const {
        return bit_lenght_;
    }

protected:
    SimpleOperand(Value *user, prim_type p_type, uint8_t bit_lenght) :
    Operand(user, opnd_t::simple), p_type_(p_type), bit_lenght_(bit_lenght) {}

    prim_type p_type_;
    uint8_t bit_lenght_;
};

class IReg : public SimpleOperand {
public:
    using idx_type = size_t;
    enum class reg_t{t, a, g, none};

    IReg(prim_type p_type, uint8_t bit_lenght, reg_t reg_type, idx_type idx, Value *user = nullptr) :
    SimpleOperand(user, p_type, bit_lenght), reg_type_(reg_type), idx_(idx) {}

    idx_type GetRegIdx() const {
        return idx_;
    }

    reg_t GetRegType() const {
        return reg_type_;
    }

    virtual ~IReg() = default;

private:
    reg_t reg_type_;
    idx_type idx_;
};

class Label final {
public:
    template <class Str>
    requires std::constructible_from<std::string, Str>
    Label(Str &&name, size_t pos = std::numeric_limits<size_t>::max()) : name_(name), pos_(pos) {}

    const std::string_view GetName() const {
        return name_;
    }

    std::string_view GetName() {
        return name_;
    }

    size_t GetPos() const {
        return pos_;
    }

    void SetPos(size_t pos) {
        pos_ = pos;
    }

private:
    std::string name_;
    size_t pos_;
};



//TODO: вывести prim_type и bit_lenght из типа
template <class T>
requires std::integral<T> || std::floating_point<T>
class Immediate final : public SimpleOperand {
public:
    using type = T;
    Immediate(prim_type p_type, uint8_t bit_lenght, T value, Value *user = nullptr) :
    SimpleOperand(user, p_type, bit_lenght), value_(value) {}

    T GetValue() const {
        return value_;
    }

    void SetValue(T value) {
        value_ = value;
    }
private:
    T value_;
};

class PhiOperand final: public Operand {
public:
    PhiOperand(Value *user, const std::string &name, std::unique_ptr<SimpleOperand> &&opnd) :
    Operand(user, opnd_t::phi), opnd_{Label(name), std::move(opnd)} {}

    PhiOperand(Value *user, const std::string &name, SimpleOperand *opnd) :
    Operand(user, opnd_t::phi), opnd_{Label(name), std::unique_ptr<SimpleOperand>(opnd)} {}

private:
    std::pair<Label, std::unique_ptr<SimpleOperand>> opnd_;
};



#endif //JIT_AOT_IN_VM_OPERANDS_H
