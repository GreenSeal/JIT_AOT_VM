//
// Created by denist on 10/1/22.
//

#ifndef VM_IR_FUNCTION_H
#define VM_IR_FUNCTION_H

#include "ir_graph.h"

class MethodInfo {};

class IRFunction : public IRGraph, MethodInfo {
public:
    IRFunction(const std::string &func_name, const std::vector<OperandBase*> &args, prim_type ret_type, const IRGraph &body) :
    IRGraph(body), ret_type_(ret_type), func_name_(func_name), args_(args) {}

    IRFunction(const std::string &func_name, const std::vector<OperandBase*> &args, prim_type ret_type, BasicBlock *body) :
    IRGraph(body, this), ret_type_(ret_type), func_name_(func_name), args_(args) {}

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

    void SetArgAt(OperandBase *arg, size_t idx);
    const OperandBase *GetArgAt(size_t idx) const;

    void PushBackArg(OperandBase *arg) {
        args_.emplace_back(arg->clone());
    }


private:
    prim_type ret_type_;
    std::string func_name_;
    std::vector<OperandBase *> args_;
};

#endif //VM_IR_FUNCTION_H
