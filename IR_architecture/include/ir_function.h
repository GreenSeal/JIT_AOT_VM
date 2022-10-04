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
            IRGraph(body), func_name_(func_name), args_(args), ret_type_(ret_type) {}

    IRFunction(const std::string &func_name, const std::vector<OperandBase*> &args, prim_type ret_type, const BasicBlock *body) :
            IRGraph(body, this), func_name_(func_name), args_(args), ret_type_(ret_type) {}

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

    void SetArgAt(OperandBase *arg, size_t idx) {
        if(idx < args_.size()) {
            throw std::invalid_argument("Invalid idx given to SetArgMethod of IRFunction class");
        }

        OperandBase *new_arg = arg->clone();
        OperandBase *old_arg = args_[idx];
        args_[idx] = new_arg;
        delete old_arg;
    }

    void PushBackArg(OperandBase *arg) {
        args_.emplace_back(arg->clone());
    }

    const OperandBase *GetArgAt(size_t idx) const {
        if(idx < args_.size()) {
            throw std::invalid_argument("Invalid idx given to GetArgAt method of IRFunction class");
        }
        return args_[idx];
    }

private:
    prim_type ret_type_;
    std::string func_name_;
    std::vector<OperandBase *> args_;
};

#endif //VM_IR_FUNCTION_H
