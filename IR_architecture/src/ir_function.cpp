//
// Created by denist on 10/4/22.
//
#include "ir_function.h"

const OperandBase *IRFunction::GetArgAt(size_t idx) const {
    if(idx < args_.size()) {
        throw std::invalid_argument("Invalid idx given to GetArgAt method of IRFunction class");
    }
    return args_[idx];
}

void IRFunction::SetArgAt(OperandBase *arg, size_t idx) {
    if(idx < args_.size()) {
        throw std::invalid_argument("Invalid idx given to SetArgMethod of IRFunction class");
    }

    OperandBase *new_arg = arg->clone();
    OperandBase *old_arg = args_[idx];
    args_[idx] = new_arg;
    delete old_arg;
}