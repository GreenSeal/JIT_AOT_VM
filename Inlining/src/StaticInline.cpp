#include "StaticInline.h"

const size_t Inlining::MAX_FUNC_SIZE = 30;

void Inlining::run(IRFunction *func) {
    auto *walker = func->GetBody()->GetFirstInst();
    while(walker != nullptr) {
        if(walker->GetInstType() == inst_t::static_call) {
            auto *call_inst = static_cast<CallInstr*>(walker);
            if(func->GetBody()->size() + call_inst->GetFunc()->GetBody()->size() < MAX_FUNC_SIZE) {}
                ReplaceCallWithBody(call_inst, call_inst->GetFunc());
        }
        walker = walker->GetNext();
    }
}

void Inlining::ReplaceCallWithBody(CallInstr *call_inst, const IRFunction *callee_func) {
    if(callee_func == nullptr) {
        return;
    }

    IRGraph *callee_body_clone = new IRGraph(*callee_func->GetBody());
    size_t max_reg = FindMaxTmpReg(call_inst->GetParentBB()->GetIRGraph());
    AddNumToEveryTmpRegIdx(callee_body_clone, max_reg+1);
    auto *cur_bb = call_inst->GetParentBB();
    cur_bb->GetIRGraph()->SplitBBInTwo(cur_bb, call_inst);

    std::vector<SimpleOperand*> func_args;
    for(size_t i = 0; i < call_inst->OpndsSize(); ++i) {
        func_args.push_back(call_inst->GetOpnd(i));
    }

    PropFuncArgs(callee_body_clone, func_args);
    InsertFuncBody(callee_body_clone, call_inst);
}

size_t Inlining::FindMaxTmpReg(const IRGraph *graph) {
    size_t max = 0;
    auto *walker = graph->GetFirstInst();
    while(walker != nullptr) {
        if(walker->GetResReg()->GetRegType() == IReg::reg_t::t
            && walker->GetResReg()->GetRegIdx() > max) {
                max = walker->GetResReg()->GetRegIdx();
        }
        walker = walker->GetNext();
    }

    return max;
}

void Inlining::AddNumToEveryTmpRegIdx(IRGraph *graph, size_t num) {
    auto *walker = graph->GetFirstInst();
    while(walker != nullptr) {
        if(walker->GetResReg()->GetRegType() == IReg::reg_t::t) {
            size_t cur_res_idx = walker->GetResReg()->GetRegIdx();
            walker->GetResReg()->SetRegIdx(cur_res_idx+num);
        }

        for(size_t i = 0; i < walker->OpndsSize(); ++i) {
            SimpleOperand *opnd;

            if(IsInstWithSimpleOpnds(walker)) {
                opnd = static_cast<SimpleOperand*>(walker->GetOpnd(i));
            } else if(walker->GetInstType() == inst_t::phi) {
                opnd = static_cast<PhiOperand*>(walker->GetOpnd(i))->GetOperand();
            } else {
                throw std::invalid_argument("Unsupported type of instruction");
            }

            if(opnd->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                auto *reg = static_cast<IReg *>(opnd);
                if(reg->GetRegType() == IReg::reg_t::t) {
                    size_t cur_opnd_idx = reg->GetRegIdx();
                    reg->SetRegIdx(cur_opnd_idx+num);
                }
            }
        }

        walker = walker->GetNext();
    }
}

void Inlining::PropFuncArgs(IRGraph *body, const std::vector<SimpleOperand*> &args) {
    Instruction *walker = body->GetFirstInst();
    while(walker != nullptr) {
        for(size_t i = 0; i<walker->OpndsSize(); ++i) {
            SimpleOperand *opnd;

            if(IsInstWithSimpleOpnds(walker)) {
                opnd = static_cast<SimpleOperand*>(walker->GetOpnd(i));
            } else if(walker->GetInstType() == inst_t::phi) {
                opnd = static_cast<PhiOperand*>(walker->GetOpnd(i))->GetOperand();
            } else {
                throw std::invalid_argument("Unsupported type of instruction");
            }

            if(opnd->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                auto *reg = static_cast<IReg *>(opnd);
                if(reg->GetRegType() == IReg::reg_t::a) {
                    size_t cur_opnd_idx = reg->GetRegIdx();
                    walker->SetOpnd(i, args.at(cur_opnd_idx)->clone());
                }
            }
        }

        walker = walker->GetNext();
    }
}

void Inlining::InsertFuncBody(IRGraph *graph, CallInstr *call_inst) {
    // inst must be the last instruction in bb and that bb must have only one predec
    std::vector<NarySimpleInstr<1> *> ret_insts;
    Instruction *walker = graph->GetFirstInst();
    while(walker != nullptr) {
        if(walker->GetInstType() == inst_t::ret) {
            ret_insts.push_back(static_cast<NarySimpleInstr<1> *>(walker));
        }
        walker = walker->GetNext();
    }

    if(call_inst->GetResReg() != nullptr) {
        BasicBlock *bb_to_insert = call_inst->GetParentBB()->GetFirstSucc();
        if(ret_insts.size() > 1) {
            PhiInst *phi_inst = new PhiInst(call_inst->GetResReg()->clone());
            for(const auto &elt: ret_insts) {
                phi_inst->PushBackOperand(elt->GetParentBB(),
                                          elt->GetOpnd(0)->clone());
            }
            bb_to_insert->InsertInstForward(phi_inst);
        } else {
            // always minimum 1
            auto *movi = new NarySimpleInstr<1>(inst_t::movi, false,
                                                call_inst->GetResReg()->clone(),
                                                ret_insts.at(0)->GetOpnd(0)->clone());
            bb_to_insert->InsertInstForward(movi);
        }
    }

    BasicBlock *end_bb = call_inst->GetParentBB()->GetFirstSucc();
    call_inst->GetParentBB()->RemoveSuccAndPredec(end_bb);
    call_inst->GetParentBB()->AddSuccWithPredec(graph->GetRoot());
    for(auto &elt: ret_insts) {
        elt->GetParentBB()->AddSuccWithPredec(end_bb);
        // TODO: Fix jumps that points into removed inst
        graph->RemoveNonCFGOrSideInst(elt);
    }

    call_inst->AssignNextAndPrev(graph->GetFirstInst());
    graph->GetLastInst()->AssignNextAndPrev(end_bb->GetFirstInst());
}
