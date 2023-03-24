//
// Created by denist on 10/1/22.
//

#ifndef VM_IR_FUNCTION_H
#define VM_IR_FUNCTION_H

#include "user.h"
#include "ir_graph.h"
#include "reg_builder.h"

class MethodInfo {};

class IRFunction : protected User<SimpleOperand, std::numeric_limits<size_t>::max()> {
public:

    template <class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    IRFunction(const std::string &func_name, std::unique_ptr<IRGraph> &&body, std::unique_ptr<Opnds> && ...args) :
    User(std::unique_ptr<IReg>(nullptr), args...), func_graph_(std::move(body)), ret_type_(prim_type::NONE), func_name_(func_name) {}

    template <class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    IRFunction(const std::string &func_name, Instruction *first, Opnds * ...args) :
    User(nullptr, args...), func_graph_(std::make_unique<IRGraph>(first, this)), ret_type_(prim_type::NONE), func_name_(func_name) {}

    template <class ...Opnds>
    requires IsPackDerivedFromSameType<SimpleOperand, Opnds...>
    IRFunction(const std::string &func_name, BasicBlock *body, Opnds *...args) :
    User(nullptr, args...), func_graph_(std::make_unique<IRGraph>(body, this)), ret_type_(prim_type::NONE), func_name_(func_name) {}

    void SetRetType(prim_type ret_type) {
        ret_type_ = ret_type;
    }

    prim_type GetRetType() const {
        return ret_type_;
    }

    void SetFuncName(const std::string &func_name) {
        func_name_ = func_name;
    }

    std::string GetFuncName() const {
        return func_name_;
    }

    const SimpleOperand *GetArgAt(size_t idx) const {
        return GetOperandAt(idx);
    }

    SimpleOperand *GetArgAt(size_t idx) {
        return GetOperandAt(idx);
    }

    const IRGraph *GetBody() const {
        return func_graph_.get();
    }

    IRGraph *GetBody() {
        return func_graph_.get();
    }

private:

    std::unique_ptr<IRGraph> func_graph_;
    prim_type ret_type_;
    std::string func_name_;
};

#endif //VM_IR_FUNCTION_H
