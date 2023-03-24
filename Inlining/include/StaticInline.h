#pragma once

#include "ir_function.h"

class Inlining {
public:
    void run(IRFunction *func);
    void ReplaceCallWithBody(CallInstr *call_inst, const IRFunction *callee_func);

    static const size_t MAX_FUNC_SIZE;
private:
    size_t FindMaxTmpReg(const IRGraph *graph);
    void AddNumToEveryTmpRegIdx(IRGraph *graph, size_t num);
    void PropFuncArgs(IRGraph *body, const std::vector<SimpleOperand*> &args);
    void InsertFuncBody(IRGraph *graph, CallInstr *call_inst);
};


