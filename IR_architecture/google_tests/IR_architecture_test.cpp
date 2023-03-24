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
    LabelOpnd label("factorial");
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
    Instruction *unary_inst = new NarySimpleInstr<1>(inst_t::movi, false,
                                                      new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                      new Immediate<uint32_t>(prim_type::UINT, 32,42));
    Instruction *binary_inst = new NarySimpleInstr<2>(inst_t::mul, false,
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1));

    EXPECT_EQ(base_inst->GetInstType(), inst_t::ret);

    EXPECT_EQ(static_cast<Jump *>(jump)->GetOpnd(0)->GetName(), "loop");

    EXPECT_EQ(static_cast<const IReg *>(unary_inst->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const Immediate<uint32_t> *>(unary_inst->GetOpnd(0))->GetValue(), 42);

    EXPECT_EQ(static_cast<const IReg *>(binary_inst->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(binary_inst->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(binary_inst->GetOpnd(1))->GetRegIdx(), 1);

    delete base_inst;
    delete jump;
    delete unary_inst;
    delete binary_inst;
}

TEST(ir_architecture, basic_block) {
    auto empty_bb = new BasicBlock();
    EXPECT_EQ(empty_bb->begin(), empty_bb->end());
    delete empty_bb;

    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp, false,
                                           new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst = new Jump("done", false, nullptr, inst_t::ja);
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul, false,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
    auto jmp_inst = new Jump("loop");

// =============== Testing forward iteration of bb, created through AddInstBack ========================
    auto bb_pushback = new BasicBlock();
    bb_pushback->SetBBName("loop");

    EXPECT_EQ(bb_pushback->GetBBName(), "loop");

    bb_pushback->InsertInstBack(cmp_inst);
    bb_pushback->InsertInstBack(ja_inst);
    bb_pushback->InsertInstBack(mul_inst);
    bb_pushback->InsertInstBack(jmp_inst);

    auto bb_pb_it = bb_pushback->begin();
    EXPECT_EQ(bb_pb_it->GetInstType(), inst_t::cmp);
    EXPECT_EQ(static_cast<const IReg *>(bb_pb_it->GetOpnd(0))->GetRegIdx(),
              1);
    EXPECT_EQ(static_cast<const IReg *>(bb_pb_it->GetOpnd(1))->GetRegIdx(),
              2);

    ++bb_pb_it;
    EXPECT_EQ(bb_pb_it->GetInstType(), inst_t::ja);
    EXPECT_EQ(static_cast<LabelOpnd *>(bb_pb_it->GetOpnd(0))->GetName(), "done");

    ++bb_pb_it;
    EXPECT_EQ(bb_pb_it->GetInstType(), inst_t::mul);
    EXPECT_EQ(static_cast<const IReg *>(bb_pb_it->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(bb_pb_it->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(bb_pb_it->GetOpnd(1))->GetRegIdx(), 1);

    ++bb_pb_it;
    EXPECT_EQ(bb_pb_it->GetInstType(), inst_t::jmp);
    EXPECT_EQ(static_cast<Jump *>(*bb_pb_it)->GetOpnd(0)->GetName(), "loop");

    ++bb_pb_it;
    EXPECT_EQ(bb_pb_it, bb_pushback->end());
// =====================================================================================================

// =============== Testing forward iteration of bb, created through instruction chain ==================
    auto cmp_inst_clone = new NarySimpleInstr<2>(inst_t::cmp, true,
                                                 new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto ja_inst_clone = new Jump("done", false, nullptr, inst_t::ja);
    auto mul_inst_clone = new NarySimpleInstr<2>(inst_t::mul, false,
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 0),
                                                 new IReg(prim_type::INT, 32, IReg::reg_t::t, 1));
    auto jmp_inst_clone = new Jump("loop");

    cmp_inst_clone->AssignNext(ja_inst_clone);
    ja_inst_clone->AssignPrev(cmp_inst_clone);
    ja_inst_clone->AssignNext(mul_inst_clone);
    mul_inst_clone->AssignPrev(ja_inst_clone);
    mul_inst_clone->AssignNext(jmp_inst_clone);
    jmp_inst_clone->AssignPrev(mul_inst_clone);

    auto bb = new BasicBlock(cmp_inst_clone, jmp_inst_clone, "loop", nullptr);

    auto bb_it = bb->begin();

    EXPECT_EQ(bb_it->GetInstType(), inst_t::cmp);
    EXPECT_EQ(static_cast<const IReg *>(bb_it->GetOpnd(0))->GetRegIdx(),
              1);
    EXPECT_EQ(static_cast<const IReg *>(bb_it->GetOpnd(1))->GetRegIdx(),
              2);

    ++bb_it;
    EXPECT_EQ(static_cast<Jump *>(*bb_it)->GetInstType(), inst_t::ja);
    EXPECT_EQ(static_cast<Jump *>(*bb_it)->GetOpnd(0)->GetName(),
              "done");

    ++bb_it;
    EXPECT_EQ(bb_it->GetInstType(), inst_t::mul);
    EXPECT_EQ(static_cast<const IReg *>(bb_it->GetResReg())->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(bb_it->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(static_cast<const IReg *>(bb_it->GetOpnd(1))->GetRegIdx(), 1);

    ++bb_it;
    EXPECT_EQ(bb_it->GetInstType(), inst_t::jmp);
    EXPECT_EQ(static_cast<Jump *>(*bb_it)->GetOpnd(0)->GetName(), "loop");

    ++bb_it;
    EXPECT_EQ(bb_it, bb->end());
// =====================================================================================================

    delete bb_pushback;
    delete bb;
}

TEST(ir_architecture, ir_graph) {

    auto movi_inst1 = new NarySimpleInstr<1>(inst_t::movi, false,
                                             new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                             new Immediate<uint32_t>(prim_type::UINT, 32, 1));
    auto movi_inst2 = new NarySimpleInstr<1>(inst_t::movi, false,
                                             new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,2));
    auto cast_inst = new NarySimpleInstr<1>(inst_t::u32tou64, false,
                                            new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                            new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0));
    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp, true,
                                           new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul, false,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1));
    auto jmp_inst = new Jump("loop", false, cmp_inst);
    auto ret_inst = new NarySimpleInstr<1>(inst_t::ret, true,
                                           new IReg(prim_type::INT, 32, IReg::reg_t::g, 0),
                                           new IReg(prim_type::INT, 32, IReg::reg_t::t, 0));
    auto ja_inst = new Jump("done", false, ret_inst, inst_t::ja);

    movi_inst1->AssignNextAndPrev(movi_inst2);
    movi_inst2->AssignNextAndPrev(cast_inst);
    cast_inst->AssignNextAndPrev(cmp_inst);
    cmp_inst->AssignNextAndPrev(ja_inst);
    ja_inst->AssignNextAndPrev(mul_inst);
    mul_inst->AssignNextAndPrev(jmp_inst);
    jmp_inst->AssignNextAndPrev(ret_inst);
// ========================== Testing IRGraph creation with ready CFG ========================
    auto ir_graph = new IRGraph(movi_inst1);
    auto first_bb = ir_graph->GetRoot();
    auto second_bb = first_bb->GetFirstSucc();
    auto third_bb = second_bb->GetFirstSucc();
    auto forth_bb = second_bb->GetSecondSucc();

    EXPECT_EQ(ir_graph->IsBBInGraph(first_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(second_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(third_bb), true);
    EXPECT_EQ(ir_graph->IsBBInGraph(forth_bb), true);

    EXPECT_EQ(ir_graph->IsBBsConnected(first_bb, second_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(second_bb, third_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(second_bb, forth_bb), true);
    EXPECT_EQ(ir_graph->IsBBsConnected(third_bb, second_bb), true);

    EXPECT_EQ(ir_graph->IsBBsConnected(third_bb, forth_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(first_bb, third_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(second_bb, first_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(forth_bb, third_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(forth_bb, second_bb), false);
    EXPECT_EQ(ir_graph->IsBBsConnected(third_bb, first_bb), false);
// ===========================================================================================

// =========== Testing IRGraph creation using AddBBInGraph and AddEdge interface ===========

//    auto movi_inst1_clone = new NarySimpleInstr<1>(inst_t::movi, false,
//                                                   new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
//                                                   new Immediate<uint32_t>(prim_type::UINT, 32,1));
//    auto movi_inst2_clone = new NarySimpleInstr<1>(inst_t::movi, false,
//                                                   new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
//                                                   new Immediate<uint32_t>(prim_type::UINT, 32,2));
//    auto cast_inst_clone = new NarySimpleInstr<1>(inst_t::u32tou64, false,
//                                                  new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
//                                                  new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
//    auto cmp_inst_clone = new NarySimpleInstr<2>(inst_t::cmp, true,
//                                                 new IReg(prim_type::UINT, 8, IReg::reg_t::t, 1),
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
//    auto ja_inst_clone = new Jump("done");
//    auto mul_inst_clone = new NarySimpleInstr<2>(inst_t::mul, false,
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
//    auto jmp_inst_clone = new Jump("loop");
//    auto ret_inst_clone = new NarySimpleInstr<1>(inst_t::ret, true,
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::g, 1),
//                                                 new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
//
//    movi_inst1_clone->AssignNextAndPrev(movi_inst2_clone);
//    movi_inst2_clone->AssignNextAndPrev(cast_inst_clone);
//    cast_inst_clone->AssignNextAndPrev(cmp_inst_clone);
//    cmp_inst_clone->AssignNextAndPrev(ja_inst_clone);
//    ja_inst_clone->AssignNextAndPrev(mul_inst_clone);
//    mul_inst_clone->AssignNextAndPrev(jmp_inst_clone);
//    jmp_inst_clone->AssignNextAndPrev(ret_inst_clone);
//
//    auto ir_graph_pushback = new IRGraph();
//
//    ir_graph_pushback->AddBBToBegin(start_bb_clone);
//    ir_graph_pushback->AddBBInGraph(loop_bb1_clone);
//    ir_graph_pushback->AddBBInGraph(loop_bb2_clone);
//    ir_graph_pushback->AddBBInGraph(done_bb_clone);
//
//    ir_graph_pushback->AddEdge(start_bb_clone, loop_bb1_clone);
//    ir_graph_pushback->AddEdge(loop_bb1_clone, loop_bb2_clone);
//    ir_graph_pushback->AddEdge(loop_bb1_clone, done_bb_clone);
//    ir_graph_pushback->AddEdge(loop_bb2_clone, loop_bb1_clone);
//    ir_graph_pushback->AddEdge(loop_bb2_clone, done_bb_clone);
//
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(start_bb_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb1_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb2_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(done_bb_clone), true);
//
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(start_bb), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb1), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(loop_bb2), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBInGraph(done_bb), false);
//
//    EXPECT_EQ(first_bb->GetBBName(), "start");
//    EXPECT_EQ(second_bb->GetBBName(), "loop1");
//    EXPECT_EQ(third_bb->GetBBName(), "loop2");
//    EXPECT_EQ(forth_bb->GetBBName(), "done");
//
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(start_bb_clone, loop_bb1_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb1_clone, loop_bb2_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb1_clone, done_bb_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb2_clone, loop_bb1_clone), true);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb2_clone, done_bb_clone), true);
//
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(start_bb_clone, loop_bb2_clone), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(loop_bb1_clone, start_bb_clone), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(done_bb_clone, loop_bb2_clone), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(done_bb_clone, loop_bb1_clone), false);
//    EXPECT_EQ(ir_graph_pushback->IsBBsConnected(done_bb_clone, start_bb_clone), false);
// ===========================================================================================

    delete ir_graph;
}

TEST(ir_architecture, ir_graph_inst_list) {
    // Testing ir_graph creation with list of instructions
    auto movi_inst1 = new NarySimpleInstr<1>(inst_t::movi, false,
                                             new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                             new Immediate<uint32_t>(prim_type::UINT, 32, 1));
    auto movi_inst2 = new NarySimpleInstr<1>(inst_t::movi, false,
                                             new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,2));
    auto cast_inst = new NarySimpleInstr<1>(inst_t::u32tou64, false,
                                            new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                            new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0));

    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp, true,
                                           new IReg(prim_type::UINT, 8, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul, false,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 1));
    auto ret_inst = new NarySimpleInstr<1>(inst_t::ret, false,
                                           new IReg(prim_type::INT, 32, IReg::reg_t::g, 0),
                                           new IReg(prim_type::INT, 32, IReg::reg_t::t, 0));
    auto jmp_inst = new Jump("loop", false, cmp_inst);
    auto ja_inst = new Jump("done", false, ret_inst, inst_t::ja);
    movi_inst1->AssignNextAndPrev(movi_inst2);
    movi_inst2->AssignNextAndPrev(cast_inst);
    cast_inst->AssignNextAndPrev(cmp_inst);
    cmp_inst->AssignNextAndPrev(ja_inst);
    ja_inst->AssignNextAndPrev(mul_inst);
    mul_inst->AssignNextAndPrev(jmp_inst);
    jmp_inst->AssignNextAndPrev(ret_inst);

    IRGraph ir_graph(movi_inst1);

    auto *first_bb = ir_graph.GetRoot();
    auto *second_bb = first_bb->GetFirstSucc();
    auto *third_bb = second_bb->GetFirstSucc();
    auto *forth_bb = second_bb->GetSecondSucc();

    EXPECT_EQ(ir_graph.IsBBInGraph(first_bb), true);
    EXPECT_EQ(ir_graph.IsBBInGraph(second_bb), true);
    EXPECT_EQ(ir_graph.IsBBInGraph(third_bb), true);
    EXPECT_EQ(ir_graph.IsBBInGraph(forth_bb), true);

    EXPECT_EQ(ir_graph.IsBBsConnected(first_bb, second_bb), true);
    EXPECT_EQ(ir_graph.IsBBsConnected(second_bb, third_bb), true);
    EXPECT_EQ(ir_graph.IsBBsConnected(second_bb, forth_bb), true);
    EXPECT_EQ(ir_graph.IsBBsConnected(third_bb, second_bb), true);

    EXPECT_EQ(ir_graph.IsBBsConnected(first_bb, third_bb), false);
    EXPECT_EQ(ir_graph.IsBBsConnected(second_bb, first_bb), false);
    EXPECT_EQ(ir_graph.IsBBsConnected(forth_bb, third_bb), false);
    EXPECT_EQ(ir_graph.IsBBsConnected(forth_bb, second_bb), false);
    EXPECT_EQ(ir_graph.IsBBsConnected(third_bb, first_bb), false);
    EXPECT_EQ(ir_graph.IsBBsConnected(third_bb, forth_bb), false);
}

TEST(ir_architecture, ir_function) {
    auto movi_inst1 = new NarySimpleInstr<2>(inst_t::movi, false,
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,1));
    auto movi_inst2 = new NarySimpleInstr<1>(inst_t::movi, false,
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                             new Immediate<uint32_t>(prim_type::UINT, 32,2));
    auto cast_inst = new NarySimpleInstr<1>(inst_t::u32tou64, false,
                                            new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                            new IReg(prim_type::INT, 32, IReg::reg_t::t, 0));
    auto cmp_inst = new NarySimpleInstr<2>(inst_t::cmp, true,
                                           new IReg(prim_type::UINT, 64, IReg::reg_t::t, 3),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2));
    auto mul_inst = new NarySimpleInstr<2>(inst_t::mul, false,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
    auto jmp_inst = new Jump("loop", false, cmp_inst);
    auto ret_inst = new NarySimpleInstr<1>(inst_t::ret, true,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::g, 0),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
    auto ja_inst = new Jump("done", false, ret_inst, inst_t::ja);

    movi_inst1->AssignNextAndPrev(movi_inst2);
    movi_inst2->AssignNextAndPrev(cast_inst);
    cast_inst->AssignNextAndPrev(cmp_inst);
    cmp_inst->AssignNextAndPrev(ja_inst);
    ja_inst->AssignNextAndPrev(mul_inst);
    mul_inst->AssignNextAndPrev(jmp_inst);
    jmp_inst->AssignNextAndPrev(ret_inst);

    SimpleOperand *arg = new IReg(prim_type::INT, 32, IReg::reg_t::t, 0);

    IRFunction func("fact", movi_inst1, arg);
    auto first_bb = func.GetBody()->GetRoot();
    auto second_bb = first_bb->GetFirstSucc();
    auto third_bb = second_bb->GetFirstSucc();
    auto forth_bb = second_bb->GetSecondSucc();

    auto *body = func.GetBody();

    EXPECT_EQ(body->IsBBsConnected(first_bb, second_bb), true);
    EXPECT_EQ(body->IsBBsConnected(second_bb, third_bb), true);
    EXPECT_EQ(body->IsBBsConnected(second_bb, forth_bb), true);
    EXPECT_EQ(body->IsBBsConnected(third_bb, second_bb), true);
    EXPECT_EQ(body->IsBBsConnected(third_bb, forth_bb), false);
}

TEST(ir_architecture, phi_instruction) {
    auto reg_1 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 0};
    auto reg_2 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 1};
    auto reg_3 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 2};
    auto reg_4 = new IReg{prim_type::UINT, 32, IReg::reg_t::t, 3};
    auto opnd_1 = new PhiOperand(reg_2, reinterpret_cast<BasicBlock*>(0x1));
    auto opnd_2 = new PhiOperand(reg_3, reinterpret_cast<BasicBlock*>(0x2));
    auto opnd_3 = new PhiOperand(reg_4, reinterpret_cast<BasicBlock*>(0x3));
    Instruction *PhiInstruction = new PhiInst(reg_1, opnd_1, opnd_2, opnd_3);

    EXPECT_EQ(static_cast<PhiOperand*>(PhiInstruction->GetOpnd(0))->GetSrcBB(), reinterpret_cast<BasicBlock*>(0x1));
    EXPECT_EQ(static_cast<PhiOperand*>(PhiInstruction->GetOpnd(1))->GetSrcBB(), reinterpret_cast<BasicBlock*>(0x2));
    EXPECT_EQ(static_cast<PhiOperand*>(PhiInstruction->GetOpnd(2))->GetSrcBB(), reinterpret_cast<BasicBlock*>(0x3));

    EXPECT_EQ(static_cast<IReg *>(static_cast<PhiOperand*>(PhiInstruction->GetOpnd(0))->GetOperand())->GetRegIdx(), 1);
    EXPECT_EQ(static_cast<IReg *>(static_cast<PhiOperand*>(PhiInstruction->GetOpnd(1))->GetOperand())->GetRegIdx(), 2);
    EXPECT_EQ(static_cast<IReg *>(static_cast<PhiOperand*>(PhiInstruction->GetOpnd(2))->GetOperand())->GetRegIdx(), 3);

    EXPECT_EQ(PhiInstruction->GetResReg()->GetRegIdx(), 0);
}






