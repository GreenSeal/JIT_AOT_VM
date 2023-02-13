//
// Created by denist on 10/1/22.
//

#ifndef VM_IR_FUNCTION_H
#define VM_IR_FUNCTION_H

#include "user.h"
#include "ir_graph.h"

class MethodInfo {};

class IRFunction : public IRGraph, protected User<SimpleOperand, std::numeric_limits<size_t>::max()> {
public:

    template <class ...Opnds>
    requires IsPackBaseOfSimpleOperand<Opnds...>
    IRFunction(const std::string &func_name, prim_type ret_type, IRGraph &&body, std::unique_ptr<Opnds> && ...args) :
    IRGraph(std::move(body)), User(prim_type::UINT, 32, 0, args...), ret_type_(ret_type), func_name_(func_name) {}

    template <class ...Opnds>
    requires IsPackBaseOfSimpleOperand<Opnds...>
    IRFunction(const std::string &func_name, prim_type ret_type, IRGraph &&body, Opnds * ...args) :
    IRGraph(std::move(body)), User(prim_type::UINT, 32, 0, std::unique_ptr<Opnds>(args)...), ret_type_(ret_type), func_name_(func_name) {}

    template <class ...Opnds>
    requires IsPackBaseOfSimpleOperand<Opnds...>
    IRFunction(const std::string &func_name, prim_type ret_type, BasicBlock *body, std::unique_ptr<Opnds> && ...args) :
    IRGraph(body, this), User(prim_type::UINT, 32, 0, args...), ret_type_(ret_type), func_name_(func_name) {}

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
        return GetOpndAt(idx);
    };

    SimpleOperand *GetArgAt(size_t idx) {
        return GetOpndAt(idx);
    }

private:
    prim_type ret_type_;
    std::string func_name_;
};

#endif //VM_IR_FUNCTION_H
