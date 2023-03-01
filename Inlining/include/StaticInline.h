#pragma once

#include "ir_function.h"

class Inlining {
public:
    void run(IRFunction *func) {
        auto *walker = func->GetBody()->GetFirstInst();
        while(walker != nullptr) {
            if(walker->GetInstType() == inst_t::call) {
                auto *call_inst = static_cast<CallInstr*>(walker);
                ReplaceCallWithBody(call_inst, call_inst->GetFunc());
            }
            walker = walker->GetNext();
        }
    }

    void ReplaceCallWithBody(CallInstr *call_inst, const IRFunction *callee_func) {
        IRGraph *callee_body_clone = new IRGraph(*callee_func->GetBody());
        size_t max_reg = FindMaxTmpReg(call_inst->GetParentBB()->GetIRGraph());
        AddNumToEveryTmpRegIdx(callee_body_clone, max_reg+1);
        auto *cur_bb = call_inst->GetParentBB();
        cur_bb->GetIRGraph()->SplitBBInTwo(cur_bb, call_inst);

        std::vector<SimpleOperand*> func_args;
        for(size_t i = 0; i < call_inst->GetOpndsSize(); ++i) {
            func_args.push_back(call_inst->GetOpnd(i));
        }

        PropFuncArgs(callee_body_clone, func_args);
        InsertFuncBody(callee_body_clone, call_inst);

        // Copy body of callee func -- done
        // Find the maximum idx of tmp register -- done
        // Add maximum register to every tmp register in function -- done
        // Split bb with call inst into two -- done
        // Propagate function arguments -- done
        // Replace call_inst with copied callee_func body and assign res_reg value of the last
        // inst in callee_inst body -- done
    }

private:
    size_t FindMaxTmpReg(const IRGraph *graph) {
        size_t max = 0;
        auto *walker = graph->GetFirstInst();
        while(walker != nullptr) {
            switch (Instruction::GetClassType(walker->GetInstType())) {
                case class_t::unary: {
                    auto cur_inst = static_cast<const NarySimpleInstr<1> *>(walker);
                    if(cur_inst->GetResReg()->GetRegType() == IReg::reg_t::t
                    && cur_inst->GetResReg()->GetRegIdx() > max) {
                        max = cur_inst->GetResReg()->GetRegIdx();
                    }
                    break;
                }
                case class_t::binary: {
                    auto cur_inst = static_cast<const NarySimpleInstr<2> *>(walker);
                    if(cur_inst->GetResReg()->GetRegType() == IReg::reg_t::t
                    && cur_inst->GetResReg()->GetRegIdx() > max) {
                        max = cur_inst->GetResReg()->GetRegIdx();
                    }
                    break;
                }
                case class_t::nary: {
                    auto cur_inst = static_cast<const NarySimpleInstr<> *>(walker);
                    if(cur_inst->GetResReg()->GetRegType() == IReg::reg_t::t
                    && cur_inst->GetResReg()->GetRegIdx() > max) {
                        max = cur_inst->GetResReg()->GetRegIdx();
                    }
                    break;
                }
                case class_t::phi: {
                    auto cur_inst = static_cast<const PhiInst *>(walker);
                    if(cur_inst->GetRes()->GetRegType() == IReg::reg_t::t
                    && cur_inst->GetRes()->GetRegIdx() > max) {
                        max = cur_inst->GetRes()->GetRegIdx();
                    }
                    break;
                }
                default:;
            }

            walker = walker->GetNext();
        }

        return max;
    }

    void AddNumToEveryTmpRegIdx(IRGraph *graph, size_t num) {
        auto *walker = graph->GetFirstInst();
        while(walker != nullptr) {
            switch(Instruction::GetClassType(walker->GetInstType())) {
                case class_t::unary: {
                    auto cur_inst = static_cast<NarySimpleInstr<1> *>(walker);
                    if(cur_inst->GetResReg()->GetRegType() == IReg::reg_t::t) {
                        size_t cur_res_idx = cur_inst->GetResReg()->GetRegIdx();
                        cur_inst->GetResReg()->SetRegIdx(cur_res_idx+num);
                    }
                    if(cur_inst->GetOpnd(0)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                        auto *reg = static_cast<IReg *>(cur_inst->GetOpnd(0));
                        if(reg->GetRegType() == IReg::reg_t::t) {
                            size_t cur_opnd_idx = reg->GetRegIdx();
                            reg->SetRegIdx(cur_opnd_idx+num);
                        }
                    }
                    break;
                }
                case class_t::binary: {
                    auto cur_inst = static_cast<NarySimpleInstr<2> *>(walker);
                    if(cur_inst->GetResReg()->GetRegType() == IReg::reg_t::t) {
                        size_t cur_res_idx = cur_inst->GetResReg()->GetRegIdx();
                        cur_inst->GetResReg()->SetRegIdx(cur_res_idx+num);
                    }
                    for(size_t i = 0; i<cur_inst->GetOpndsSize(); ++i) {
                        if(cur_inst->GetOpnd(i)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                            auto *reg = static_cast<IReg *>(cur_inst->GetOpnd(i));
                            if(reg->GetRegType() == IReg::reg_t::t) {
                                size_t cur_opnd_idx = reg->GetRegIdx();
                                reg->SetRegIdx(cur_opnd_idx+num);
                            }
                        }
                    }
                    break;
                }
                case class_t::phi: {
                    auto cur_inst = static_cast<PhiInst *>(walker);
                    if(cur_inst->GetRes()->GetRegType() == IReg::reg_t::t) {
                        size_t cur_res_idx = cur_inst->GetRes()->GetRegIdx();
                        cur_inst->GetRes()->SetRegIdx(cur_res_idx+num);
                    }
                    for(size_t i = 0; i<cur_inst->GetOperandsSize(); ++i) {
                        if(cur_inst->GetOperandAt(i)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                            auto *reg = static_cast<IReg *>(cur_inst->GetOperandAt(i));
                            if(reg->GetRegType() == IReg::reg_t::t) {
                                size_t cur_opnd_idx = reg->GetRegIdx();
                                reg->SetRegIdx(cur_opnd_idx+num);
                            }
                        }
                    }
                    break;
                }
                case class_t::nary: {
                    auto cur_inst = static_cast<NarySimpleInstr<> *>(walker);
                    if(cur_inst->GetResReg()->GetRegType() == IReg::reg_t::t) {
                        size_t cur_res_idx = cur_inst->GetResReg()->GetRegIdx();
                        cur_inst->GetResReg()->SetRegIdx(cur_res_idx+num);
                    }
                    for(size_t i = 0; i<cur_inst->GetOpndsSize(); ++i) {
                        if(cur_inst->GetOpnd(i)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                            auto *reg = static_cast<IReg *>(cur_inst->GetOpnd(i));
                            if(reg->GetRegType() == IReg::reg_t::t) {
                                size_t cur_opnd_idx = reg->GetRegIdx();
                                reg->SetRegIdx(cur_opnd_idx+num);
                            }
                        }
                    }
                    break;
                }
                default:;
            }
            walker = walker->GetNext();
        }
    }

    void PropFuncArgs(IRGraph *body, const std::vector<SimpleOperand*> &args) {
        Instruction *walker = body->GetFirstInst();
        while(walker != nullptr) {
            switch(Instruction::GetClassType(walker->GetInstType())) {
                case class_t::unary: {
                    auto cur_inst = static_cast<NarySimpleInstr<1> *>(walker);
                    if(cur_inst->GetOpnd(0)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                        auto *reg = static_cast<IReg *>(cur_inst->GetOpnd(0));
                        if(reg->GetRegType() == IReg::reg_t::a) {
                            size_t cur_opnd_idx = reg->GetRegIdx();
                            cur_inst->SetOpnd(0, args.at(cur_opnd_idx)->clone());
                        }
                    }
                    break;
                }
                case class_t::binary: {
                    auto cur_inst = static_cast<NarySimpleInstr<2> *>(walker);
                    for(size_t i = 0; i<cur_inst->GetOpndsSize(); ++i) {
                        if(cur_inst->GetOpnd(i)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                            auto *reg = static_cast<IReg *>(cur_inst->GetOpnd(i));
                            if(reg->GetRegType() == IReg::reg_t::a) {
                                size_t cur_opnd_idx = reg->GetRegIdx();
                                cur_inst->SetOpnd(i, args.at(cur_opnd_idx)->clone());
                            }
                        }
                    }
                    break;
                }
                case class_t::phi: {
                    auto cur_inst = static_cast<PhiInst *>(walker);
                    for(size_t i = 0; i<cur_inst->GetOperandsSize(); ++i) {
                        if(cur_inst->GetOperandAt(i)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                            auto *reg = static_cast<IReg *>(cur_inst->GetOperandAt(i));
                            if(reg->GetRegType() == IReg::reg_t::a) {
                                size_t cur_opnd_idx = reg->GetRegIdx();
                                cur_inst->SetOperandAt(i, args.at(cur_opnd_idx)->clone());
                            }
                        }
                    }
                    break;
                }
                case class_t::nary: {
                    auto cur_inst = static_cast<NarySimpleInstr<> *>(walker);
                    for(size_t i = 0; i<cur_inst->GetOpndsSize(); ++i) {
                        if(cur_inst->GetOpnd(i)->GetSimpleOpndType() == SimpleOperand::simple_opnd_t::ireg) {
                            auto *reg = static_cast<IReg *>(cur_inst->GetOpnd(i));
                            if(reg->GetRegType() == IReg::reg_t::a) {
                                size_t cur_opnd_idx = reg->GetRegIdx();
                                cur_inst->SetOpndAt(i, args.at(cur_opnd_idx)->clone());
                            }
                        }
                    }
                    break;
                }
                default:;
            }
            walker = walker->GetNext();
        }
    }

    void InsertFuncBody(IRGraph *graph, CallInstr *call_inst) {
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
};


