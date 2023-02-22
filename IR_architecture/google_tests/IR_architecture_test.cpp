//
// Created by denist on 10/2/22.
//

#include <gtest/gtest.h>
#include "instructions.h"
#include "basic_block.h"
#include "ilist_nodes.h"
#include "ir_function.h"
#include "ir_graph.h"
#include "operands.h"
#include "reg_builder.h"

TEST(ir_architecture, operands) {
    SimpleOperand *reg = new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0);
    Label label("factorial");
    SimpleOperand *uint32_const = new Immediate<uint32_t>(prim_type::UINT, 32,42);

    EXPECT_EQ(static_cast<IReg*>(reg)->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<IReg*>(reg)->GetRegType(), IReg::reg_t::t);

    EXPECT_EQ(label.GetName(), "factorial");

    EXPECT_EQ(static_cast<Immediate<uint32_t>*>(uint32_const)->GetValue(), 42);


    delete reg;
    delete uint32_const;
}

TEST(ir_architecture, instructions) {
    //RegBuilder reg_builder;
    
    Instruction *base_inst = new Instruction(inst_t::ret, prim_type::NONE);
    Instruction *jump = new Jump("loop");
    Instruction *unary_inst = new NarySimpleInstr<1>(inst_t::movi,
                                                      new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                      new Immediate<uint32_t>(prim_type::UINT, 32,42));
    Instruction *binary_inst = new NarySimpleInstr<2>(inst_t::mul,
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1));

    EXPECT_EQ(base_inst->GetInstType(), inst_t::ret);

    EXPECT_EQ(static_cast<Jump *>(jump)->GetLabelName(), "loop");

    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<1> *>(unary_inst)->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const Immediate<uint32_t> *>(static_cast<NarySimpleInstr<1> *>(unary_inst)->GetOpndAt(0))->GetValue(), 42);

    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(binary_inst)->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(binary_inst)->GetOpndAt(0))->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(binary_inst)->GetOpndAt(1))->GetRegIdx(), 1);

    delete base_inst;
    delete jump;
    delete unary_inst;
    delete binary_inst;
}

TEST(ir_architecture, basic_block) {
    auto empty_bb = new BasicBlock();
    EXPECT_EQ(empty_bb->begin(), empty_bb->end());
    delete empty_bb;

    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp,
                                           new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst = new Jump("done", inst_t::ja);
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
    auto jmp_inst = new Jump("loop");

// =============== Testing forward iteration of bb, created through AddInstBack ========================
    auto bb_pushback = new BasicBlock();
    bb_pushback->SetBBName("loop");

    EXPECT_EQ(bb_pushback->GetBBName(), "loop");

    bb_pushback->ReplaceInstBack(cmp_inst);
    bb_pushback->ReplaceInstBack(ja_inst);
    bb_pushback->ReplaceInstBack(mul_inst);
    bb_pushback->ReplaceInstBack(jmp_inst);

    auto bb_pb_it = bb_pushback->begin();
    EXPECT_EQ(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetInstType(), inst_t::cmp);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetOpnd(0))->GetRegIdx(),
              1);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetOpnd(1))->GetRegIdx(),
              2);

    ++bb_pb_it;
    EXPECT_EQ(static_cast<Jump *>(*bb_pb_it)->GetInstType(), inst_t::ja);
    EXPECT_EQ(static_cast<Jump *>(*bb_pb_it)->GetLabelName(),
              "done");

    ++bb_pb_it;
    EXPECT_EQ(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetInstType(), inst_t::mul);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_pb_it)->GetOpnd(1))->GetRegIdx(), 1);

    ++bb_pb_it;
    EXPECT_EQ(static_cast<Jump *>(*bb_pb_it)->GetInstType(), inst_t::jmp);
    EXPECT_EQ(static_cast<Jump *>(*bb_pb_it)->GetLabelName(), "loop");

    ++bb_pb_it;
    EXPECT_EQ(bb_pb_it, bb_pushback->end());
// =====================================================================================================

// =============== Testing forward iteration of bb, created through instruction chain ==================
    auto cmp_inst_clone = new NarySimpleInstr<2>(inst_t::cmp,
                                                 new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst_clone = new Jump("done", inst_t::ja);
    auto mul_inst_clone = new NarySimpleInstr<2>(inst_t::mul,
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 1));
    auto jmp_inst_clone = new Jump("loop");

    cmp_inst_clone->SetNext(ja_inst_clone);
    ja_inst_clone->SetPrev(cmp_inst_clone);
    ja_inst_clone->SetNext(mul_inst_clone);
    mul_inst_clone->SetPrev(ja_inst_clone);
    mul_inst_clone->SetNext(jmp_inst_clone);
    jmp_inst_clone->SetPrev(mul_inst_clone);

    auto bb = new BasicBlock(nullptr, "loop", cmp_inst_clone);

    auto bb_it = bb->begin();

    EXPECT_EQ(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetInstType(), inst_t::cmp);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetOpnd(0))->GetRegIdx(),
              1);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetOpnd(1))->GetRegIdx(),
              2);

    ++bb_it;
    EXPECT_EQ(static_cast<Jump *>(*bb_it)->GetInstType(), inst_t::ja);
    EXPECT_EQ(static_cast<Jump *>(*bb_it)->GetLabelName(),
              "done");

    ++bb_it;
    EXPECT_EQ(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetInstType(), inst_t::mul);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(static_cast<NarySimpleInstr<2> *>(*bb_it)->GetOpnd(1))->GetRegIdx(), 1);

    ++bb_it;
    EXPECT_EQ(static_cast<NarySimpleInstr<1> *>(*bb_it)->GetInstType(), inst_t::jmp);
    EXPECT_EQ(static_cast<Jump *>(*bb_it)->GetLabelName(), "loop");

    ++bb_it;
    EXPECT_EQ(bb_it, bb->end());
// =====================================================================================================

    delete bb_pushback;
    delete bb;
}

TEST(ir_architecture, ir_graph) {

    auto movi_inst1 = new NarySimpleInstr<1>(inst_t::movi,
                                             new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                             new Immediate<uint32_t>(prim_type::UINT, 32, 1));
    auto movi_inst2 = new NarySimpleInstr<1>(inst_t::movi,
                                             new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,2));
    auto cast_inst = new NarySimpleInstr<1>(inst_t::u32tou64,
                                            new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                            new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0));
    auto start_bb = new BasicBlock(nullptr, "start");

    start_bb->ReplaceInstBack(movi_inst1);
    start_bb->ReplaceInstBack(movi_inst2);
    start_bb->ReplaceInstBack(cast_inst);

    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp,
                                           new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst = new Jump("done");
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1));
    auto jmp_inst = new Jump("loop");

    auto loop_bb1 = new BasicBlock(nullptr, "loop1");
    auto loop_bb2 = new BasicBlock(nullptr, "loop2");

    loop_bb1->ReplaceInstBack(cmp_inst);
    loop_bb1->ReplaceInstBack(ja_inst);
    loop_bb2->ReplaceInstBack(mul_inst);
    loop_bb2->ReplaceInstBack(jmp_inst);

    auto ret_inst = new NarySimpleInstr<1>(inst_t::ret,
                                           new IReg(prim_type::INT, 32, IReg::reg_t::g, 0),
                                           new IReg(prim_type::INT, 32, IReg::reg_t::t, 0));

    auto done_bb = new BasicBlock(nullptr, "done");
    done_bb->ReplaceInstBack(ret_inst);
// ========================== Testing IRGraph creation with ready CFG ========================
    start_bb->AddSucc(loop_bb1);
    loop_bb1->AddPredec(loop_bb2);
    loop_bb1->AddPredec(start_bb);
    loop_bb1->AddSucc(loop_bb2);
    loop_bb1->AddSucc(done_bb);
    loop_bb2->AddPredec(loop_bb1);
    loop_bb2->AddSucc(loop_bb1);
    loop_bb2->AddSucc(done_bb);
    done_bb->AddPredec(loop_bb2);
    done_bb->AddPredec(loop_bb1);

    auto ir_graph = new IRGraph(start_bb);
    auto first_bb = ir_graph->GetRoot();
    auto second_bb = first_bb->GetFirstSucc();
    auto third_bb = second_bb->GetFirstSucc();
    auto forth_bb = second_bb->GetSecondSucc();

    EXPECT_EQ(ir_graph->IsBBInGraph(first_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(second_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(third_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(forth_bb), true);

    EXPECT_EQ(ir_graph->IsBBInGraph(start_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(loop_bb1), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(loop_bb2), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(done_bb), true);

    EXPECT_EQ(first_bb->GetBBName(), "start");
    EXPECT_EQ(second_bb->GetBBName(), "loop1");
    EXPECT_EQ(third_bb->GetBBName(), "loop2");
    EXPECT_EQ(forth_bb->GetBBName(), "done");

    EXPECT_EQ(ir_graph->IsBBsConnected(first_bb, second_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(second_bb, third_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(second_bb, forth_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(third_bb, second_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(third_bb, forth_bb), true);

    EXPECT_EQ(ir_graph->IsBBsConnected(first_bb, third_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(second_bb, first_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(forth_bb, third_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(forth_bb, second_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(third_bb, first_bb), false);
// ===========================================================================================

// =========== Testing IRGraph creation using AddBBInGraph and AddEdge interface ===========

    auto movi_inst1_clone = new NarySimpleInstr<1>(inst_t::movi,
                                                   new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                                   new Immediate<uint32_t>(prim_type::UINT, 32,1));
    auto movi_inst2_clone = new NarySimpleInstr<1>(inst_t::movi,
                                                   new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                                   new Immediate<uint32_t>(prim_type::UINT, 32,2));
    auto cast_inst_clone = new NarySimpleInstr<1>(inst_t::u32tou64,
                                                  new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                                  new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
    auto start_bb_clone = new BasicBlock(nullptr, "start");

    start_bb_clone->ReplaceInstBack(movi_inst1_clone);
    start_bb_clone->ReplaceInstBack(movi_inst2_clone);
    start_bb_clone->ReplaceInstBack(cast_inst_clone);

    auto cmp_inst_clone = new NarySimpleInstr<2>(inst_t::cmp,
                                                 new IReg(prim_type::UINT, 8, IReg::reg_t::t, 1),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst_clone = new Jump("done");
    auto mul_inst_clone = new NarySimpleInstr<2>(inst_t::mul,
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
    auto jmp_inst_clone = new Jump("loop");

    auto loop_bb1_clone = new BasicBlock(nullptr, "loop1");
    auto loop_bb2_clone = new BasicBlock(nullptr, "loop2");

    loop_bb1_clone->ReplaceInstBack(cmp_inst_clone);
    loop_bb1_clone->ReplaceInstBack(ja_inst_clone);
    loop_bb2_clone->ReplaceInstBack(mul_inst_clone);
    loop_bb2_clone->ReplaceInstBack(jmp_inst_clone);

    auto ret_inst_clone = new NarySimpleInstr<1>(inst_t::ret,
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::g, 1),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));

    auto done_bb_clone = new BasicBlock(nullptr, "done");
    done_bb_clone->ReplaceInstBack(ret_inst_clone);

    auto ir_graph_pushback = new IRGraph();

    ir_graph_pushback->AddBBToBegin(start_bb_clone);
    ir_graph_pushback->AddBBInGraph(loop_bb1_clone);
    ir_graph_pushback->AddBBInGraph(loop_bb2_clone);
    ir_graph_pushback->AddBBInGraph(done_bb_clone);

    ir_graph_pushback->AddEdge(start_bb_clone, loop_bb1_clone);
    ir_graph_pushback->AddEdge(loop_bb1_clone, loop_bb2_clone);
    ir_graph_pushback->AddEdge(loop_bb1_clone, done_bb_clone);
    ir_graph_pushback->AddEdge(loop_bb2_clone, loop_bb1_clone);
    ir_graph_pushback->AddEdge(loop_bb2_clone, done_bb_clone);
//    root->AddSucc(loop_bb1_clone);
//    loop_bb1_clone->AddPredec(root);
//    loop_bb1_clone->AddSucc(loop_bb2_clone);
//    loop_bb2_clone->AddPredec(loop_bb1_clone);
//    loop_bb1_clone->AddSucc(done_bb_clone);
//    done_bb_clone->AddPredec(loop_bb1_clone);
//    loop_bb2_clone->AddSucc(loop_bb1_clone);
//    loop_bb1_clone->AddPredec(loop_bb2_clone);
//    loop_bb2_clone->AddSucc(done_bb_clone);
//    done_bb_clone->AddPredec(loop_bb2_clone);

    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(start_bb_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb1_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb2_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(done_bb_clone), true);

    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(start_bb), false);
    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb1), false);
    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb2), false);
    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(done_bb), false);

    EXPECT_EQ(first_bb->GetBBName(), "start");
    EXPECT_EQ(second_bb->GetBBName(), "loop1");
    EXPECT_EQ(third_bb->GetBBName(), "loop2");
    EXPECT_EQ(forth_bb->GetBBName(), "done");

    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(start_bb_clone, loop_bb1_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb1_clone, loop_bb2_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb1_clone, done_bb_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb2_clone, loop_bb1_clone), true);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb2_clone, done_bb_clone), true);

    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(start_bb_clone, loop_bb2_clone), false);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb1_clone, start_bb_clone), false);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(done_bb_clone, loop_bb2_clone), false);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(done_bb_clone, loop_bb1_clone), false);
    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(done_bb_clone, start_bb_clone), false);
// ===========================================================================================

    delete ir_graph;
    delete ir_graph_pushback;
}

TEST(ir_architecture, ir_function) {
    auto movi_inst1 = new NarySimpleInstr<2>(inst_t::movi,
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,1));
    auto movi_inst2 = new NarySimpleInstr<1>(inst_t::movi,
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,2));
    auto cast_inst = new NarySimpleInstr<1>(inst_t::u32tou64,
                                            new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                            new IReg(prim_type::INT, 32, IReg::reg_t::t, 0));
    auto start_bb = new BasicBlock(nullptr, "start");
    start_bb->ReplaceInstBack(movi_inst1);
    start_bb->ReplaceInstBack(movi_inst2);
    start_bb->ReplaceInstBack(cast_inst);

    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp,
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 3),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst = new Jump("done");
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
    auto jmp_inst = new Jump("loop");

    auto loop_bb1 = new BasicBlock(nullptr, "loop1");
    auto loop_bb2 = new BasicBlock(nullptr, "loop2");
    loop_bb1->ReplaceInstBack(cmp_inst);
    loop_bb1->ReplaceInstBack(ja_inst);
    loop_bb2->ReplaceInstBack(mul_inst);
    loop_bb2->ReplaceInstBack(jmp_inst);

    auto ret_inst = new NarySimpleInstr<1>(inst_t::ret,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::g, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));

    auto done_bb = new BasicBlock(nullptr, "done");
    done_bb->ReplaceInstBack(ret_inst);

    start_bb->AddSucc(loop_bb1);
    loop_bb1->AddPredec(loop_bb2);
    loop_bb1->AddPredec(start_bb);
    loop_bb1->AddSucc(loop_bb2);
    loop_bb1->AddSucc(done_bb);
    loop_bb2->AddPredec(loop_bb1);
    loop_bb2->AddSucc(loop_bb1);
    loop_bb2->AddSucc(done_bb);
    done_bb->AddPredec(loop_bb2);
    done_bb->AddPredec(loop_bb1);

    SimpleOperand *arg = new IReg(prim_type::INT, 32, IReg::reg_t::t, 0);

    IRFunction func("fact", start_bb, arg);
    auto first_bb = func.GetRoot();
    auto second_bb = first_bb->GetFirstSucc();
    auto third_bb = second_bb->GetFirstSucc();
    auto forth_bb = second_bb->GetSecondSucc();

    EXPECT_EQ(func.IsBBsConnected(first_bb, second_bb), true);
    EXPECT_EQ(func.IsBBsConnected(second_bb, third_bb), true);
    EXPECT_EQ(func.IsBBsConnected(second_bb, forth_bb), true);
    EXPECT_EQ(func.IsBBsConnected(third_bb, second_bb), true);
    EXPECT_EQ(func.IsBBsConnected(third_bb, forth_bb), true);
}

TEST(ir_architecture, phi_instruction) {
    auto reg_1 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 0};
    auto reg_2 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 1};
    auto reg_3 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 2};
    auto reg_4 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 3};
    auto opnd_1 = new PhiOperand(nullptr, "label1", reg_2);
    auto opnd_2 = new PhiOperand(nullptr, "label2", reg_3);
    auto opnd_3 = new PhiOperand(nullptr, "label3", reg_4);
    auto PhiInstruction = new PhiInst(reg_1, opnd_1, opnd_2, opnd_3);

    EXPECT_EQ(PhiInstruction->GetLabelAt(0), "label1");
    EXPECT_EQ(PhiInstruction->GetLabelAt(1), "label2");
    EXPECT_EQ(PhiInstruction->GetLabelAt(2), "label3");

    EXPECT_EQ(static_cast<IReg *>(PhiInstruction->GetOperandAt(0))->GetRegIdx(), 1);
    EXPECT_EQ(static_cast<IReg *>(PhiInstruction->GetOperandAt(1))->GetRegIdx(), 2);
    EXPECT_EQ(static_cast<IReg *>(PhiInstruction->GetOperandAt(2))->GetRegIdx(), 3);

    EXPECT_EQ(PhiInstruction->GetRes()->GetRegIdx(), 0);
}






