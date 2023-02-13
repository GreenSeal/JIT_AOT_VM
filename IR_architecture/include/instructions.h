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

    Instruction(const Instruction &other) = delete;
    Instruction &operator=(const Instruction &other) = delete;

    Instruction(Instruction &&other) = delete;
    Instruction &&operator=(Instruction &&other) = delete;

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

protected:

    inst_t type_;
    prim_type prim_type_;
};


// TODO: nontrivial initialization of result operand
// TODO: after that rewrite tests
template <size_t opnds_num = std::numeric_limits<size_t>::max()>
class NarySimpleInstr : public Instruction, public User<SimpleOperand, opnds_num> {
public:
    template<class ...Opnds>
    requires IsPackBaseOfSimpleOperand<Opnds...>
    NarySimpleInstr(inst_t type, prim_type p_type, std::unique_ptr<Opnds> &&... opnd) :
    Instruction(type, p_type), User<SimpleOperand, opnds_num>(p_type, 32, 0, opnd...) {}

    template<class ...Opnds>
    requires IsPackBaseOfSimpleOperand<Opnds...>
    NarySimpleInstr(inst_t type, prim_type p_type, Opnds *... opnd) :
            Instruction(type, p_type), User<SimpleOperand, opnds_num>(p_type, 32, 0, std::unique_ptr<Opnds>(opnd)...) {}

    NarySimpleInstr(const NarySimpleInstr &rhs) = delete;
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

    std::string GetLabelName() {
        return label_.GetName();
    }

protected:
    Label label_;
};
template <class ...Elts>
concept is_pack_of_phipairs = (std::same_as<Elts, std::pair<Label, std::unique_ptr<SimpleOperand>>> && ...);

class PhiInst final : public Instruction,
        User<std::pair<Label, std::unique_ptr<SimpleOperand>>, std::numeric_limits<size_t>::max()> {
    using PhiPair = OperandType;

public:
    template <class ...Args>
    requires is_pack_of_phipairs<Args...>
    PhiInst(prim_type p_type, std::unique_ptr<Args> && ...args) : Instruction(inst_t::phi, p_type),
    User<PhiPair, std::numeric_limits<size_t>::max()>(args...) {}

    template<class ...Args>
    requires is_pack_of_phipairs<Args...>
    PhiInst(prim_type p_type, Args * ...args) : Instruction(inst_t::phi, p_type),
    User<PhiPair, std::numeric_limits<size_t>::max()>(args...) {}
};


#endif //JIT_AOT_IN_VM_INSTRUCTIONS_H
