//
// Created by denist on 9/26/22.
//

#ifndef JIT_AOT_IN_VM_OPERANDS_H
#define JIT_AOT_IN_VM_OPERANDS_H

enum class opnd_t {ireg, label};

class OperandBase {
public:
    OperandBase(opnd_t type) : type_(type) {}
    virtual ~OperandBase() {}
private:
    opnd_t type_;
};

class IReg final : public OperandBase {
public:
    using idx_type = size_t;
    IRef(idx_type idx) : OperandBase(opnd_t::ireg), idx_(idx) {}

private:
    idx_type idx_;
};

class Label final : public OperandBase {
public:
    Label(const std::string &name) : OperandBase(opnd_t::label), name_(name) {}
private:
    std::string name_;
};

#endif //JIT_AOT_IN_VM_OPERANDS_H
