//
// Created by denist on 2/22/23.
//

#include <gtest/gtest.h>

#include "StaticInline.h"

TEST(static_inline, func_with_if) {
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
                                             nullptr,
                                             new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
    auto *second_ret = new NarySimpleInstr<1>(inst_t::ret, true,
                                              nullptr,
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
                                            nullptr,
                                            new IReg(prim_type::UINT, 32, IReg::reg_t::t, 0));
    new_movi->AssignNextAndPrev(new_mult);
    new_mult->AssignNextAndPrev(new_call);
    new_call->AssignNextAndPrev(new_ret);

    auto *caller = new IRFunction("caller", new_movi);

}

