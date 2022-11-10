//
// Created by denist on 9/26/22.
//

#ifndef JIT_AOT_IN_VM_OPERANDS_H
#define JIT_AOT_IN_VM_OPERANDS_H

enum class opnd_t {ireg, label, imm};
enum class prim_type {u32, u64, none};

class OperandBase {
public:
    OperandBase(opnd_t type) : type_(type) {}

    virtual OperandBase *clone() const {
        return new OperandBase(*this);
    }

    virtual ~OperandBase() {}

    opnd_t GetType() const {
        return type_;
    }

    void SetType(opnd_t type) {
        type_ = type;
    }

private:
    opnd_t type_;
};

class IReg final : public OperandBase {
public:
    using idx_type = size_t;
    enum class reg_t{v, a};

    IReg(reg_t reg_type, idx_type idx, prim_type value_type) :
    OperandBase(opnd_t::ireg), value_type_(value_type), reg_type_(reg_type), idx_(idx) {}

    virtual IReg *clone() const override {
        return new IReg(*this);
    }

    prim_type GetRegValueType() const {
        return value_type_;
    }

    void SetRegValueType(prim_type value_type) {
        value_type_ = value_type;
    }

    reg_t GetRegType() const {
        return reg_type_;
    }

    void SetRegType(reg_t reg_type) {
        reg_type_ = reg_type;
    }

    idx_type GetRegIdx() const {
        return idx_;
    }

    void SetRegIdx(idx_type idx) {
        idx_ = idx;
    }
private:
    prim_type value_type_;
    reg_t reg_type_;
    idx_type idx_;
};

class Label final : public OperandBase {
public:
    Label(const std::string &name) : OperandBase(opnd_t::label), name_(name) {}

    virtual Label *clone() const override {
        return new Label(*this);
    }

    std::string GetLabelName() const {
        return name_;
    }

    void SetLabelName(const std::string &name) {
        name_ = name;
    }

private:
    std::string name_;
};

class UInt32Const final : public OperandBase {
public:
    UInt32Const(uint32_t value) : OperandBase(opnd_t::imm), value_(value) {}

    virtual UInt32Const *clone() const override {
        return new UInt32Const(*this);
    }

    uint32_t GetUInt32Value() const {
        return value_;
    }

    void SetConstUint32Value(uint32_t value) {
        value_ = value;
    }
private:
    uint32_t value_;
};

#endif //JIT_AOT_IN_VM_OPERANDS_H
