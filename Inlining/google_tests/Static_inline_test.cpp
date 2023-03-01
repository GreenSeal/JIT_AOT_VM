//
// Created by denist on 2/22/23.
//

#include <gtest/gtest.h>

#include "StaticInline.h"

TEST(static_inline, func_with_two_rets) {
    auto *movi = new NarySimpleInstr<1>(inst_t::movi, false,
                                        new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                        new IReg(prim_type::UINT, 32, IReg::reg_t::a, 0));
    auto *add = new NarySimpleInstr<2>(inst_t::add, false,
                                       new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                       new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                       new IReg(prim_type::UINT, 32, IReg::reg_t::a, 1));
    auto *cmp = new NarySimpleInstr<2>(inst_t::cmp, false,
                                       new IReg(prim_type::UINT, 32, IReg::reg_t::g, 0),
                                       new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                       new Immediate<uint32_t>(prim_type::UINT, 32, 4));
    auto *first_ret = new NarySimpleInstr<1>(inst_t::ret, false,
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::g, 1),
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
    auto *second_ret = new NarySimpleInstr<1>(inst_t::ret, true,
                                              new IReg(prim_type::UINT, 32, IReg::reg_t::g, 1),
                                              new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1));
    auto *ja = new Jump("1", second_ret, inst_t::ja);

    movi->AssignNextAndPrev(add);
    add->AssignNextAndPrev(cmp);
    cmp->AssignNextAndPrev(ja);
    ja->AssignNextAndPrev(first_ret);
    first_ret->AssignNextAndPrev(second_ret);

    IRFunction *callee = new IRFunction("callee", movi,
                                        new IReg(prim_type::UINT, 32, IReg::reg_t::a, 0),
                                        new IReg(prim_type::UINT, 32, IReg::reg_t::a, 1));

    auto *new_movi = new NarySimpleInstr<1>(inst_t::movi, false,
                                            new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                            new Immediate<uint32_t>(prim_type::UINT, 32, 6));
    auto *new_mult = new NarySimpleInstr<2>(inst_t::mul, false,
                                            new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                            new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0),
                                            new Immediate<uint32_t>(prim_type::UINT, 32, 12));
    auto *new_call = new CallInstr(false, callee,
                                   new IReg(prim_type::UINT, 32, IReg::reg_t::t, 2),
                                   new IReg(prim_type::UINT, 32, IReg::reg_t::t, 1),
                                   new Immediate<uint32_t>(prim_type::UINT, 32, 42));
    auto *new_ret = new NarySimpleInstr<1>(inst_t::ret, false,
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::g, 1),
                                           new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
    new_movi->AssignNextAndPrev(new_mult);
    new_mult->AssignNextAndPrev(new_call);
    new_call->AssignNextAndPrev(new_ret);

    auto *caller = new IRFunction("caller", new_movi);

    auto *inline_algo = new Inlining();

    EXPECT_TRUE(caller);
    EXPECT_TRUE(inline_algo);
    inline_algo->run(caller);

    // Check idx changing, data flow correction and caller func body changing
    auto *next = new_call->GetNext();
    ASSERT_EQ(next->GetInstType(), inst_t::movi);
    auto *cloned_movi = static_cast<NarySimpleInstr<1>*>(next);
    EXPECT_EQ(cloned_movi->GetResReg()->GetRegIdx(), 3);
    EXPECT_EQ(static_cast<IReg*>(cloned_movi->GetOpnd(0))->GetRegType(), IReg::reg_t::t);
    EXPECT_EQ(static_cast<IReg*>(cloned_movi->GetOpnd(0))->GetRegIdx(), 1);

    next = next->GetNext();
    ASSERT_EQ(next->GetInstType(), inst_t::add);
    auto *cloned_add = static_cast<NarySimpleInstr<2>*>(next);
    EXPECT_EQ(cloned_add->GetResReg()->GetRegIdx(), 4);
    EXPECT_EQ(static_cast<IReg*>(cloned_add->GetOpnd(0))->GetRegIdx(), 3);
    EXPECT_EQ(static_cast<Immediate<uint32_t>*>(cloned_add->GetOpnd(1))->GetValue(), 42);

    next = next->GetNext();
    ASSERT_EQ(next->GetInstType(), inst_t::cmp);
    auto *cloned_cmp = static_cast<NarySimpleInstr<2>*>(next);
    EXPECT_EQ(static_cast<IReg*>(cloned_cmp->GetOpnd(0))->GetRegIdx(), 4);

    next = next->GetNext();
    ASSERT_EQ(next->GetInstType(), inst_t::ja);

    next = next->GetNext();
    ASSERT_EQ(next->GetInstType(), inst_t::phi);
    auto *created_phi = static_cast<PhiInst*>(next);
    EXPECT_EQ(created_phi->GetRes()->GetRegIdx(), 2);
    EXPECT_EQ(static_cast<IReg*>(created_phi->GetOperandAt(0))->GetRegIdx(), 3);
    EXPECT_EQ(static_cast<IReg*>(created_phi->GetOperandAt(1))->GetRegIdx(), 4);

    next = next->GetNext();
    ASSERT_EQ(next->GetInstType(), inst_t::ret);

    // Check CFG
    auto *call_inst_bb = new_call->GetParentBB();
    EXPECT_EQ(call_inst_bb->GetFirstSucc(), new_call->GetNext()->GetParentBB());
    EXPECT_EQ(call_inst_bb->GetSecondSucc(), nullptr);

    auto *first_inlined_bb = call_inst_bb->GetFirstSucc();
    auto *false_if = first_inlined_bb->GetFirstSucc();
    auto *true_if = first_inlined_bb->GetSecondSucc();
    EXPECT_EQ(false_if->GetFirstInst(), nullptr);
    EXPECT_EQ(true_if->GetFirstInst(), nullptr);

    EXPECT_EQ(true_if->GetFirstSucc(), false_if->GetFirstSucc());
    EXPECT_EQ(true_if->GetSecondSucc(), nullptr);
    EXPECT_EQ(false_if->GetSecondSucc(), nullptr);

    EXPECT_EQ(true_if->GetFirstSucc()->GetFirstInst()->GetInstType(), inst_t::phi);
}

