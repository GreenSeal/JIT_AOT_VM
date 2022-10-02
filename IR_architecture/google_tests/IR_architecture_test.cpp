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

TEST(ir_architecture, operands) {
    OperandBase *reg = new IReg(IReg::reg_t::v, 0, prim_type::u32);
    OperandBase *label = new Label("factorial");
    OperandBase *uint32_const = new UInt32Const(42);

    EXPECT_EQ(dynamic_cast<IReg*>(reg)->GetRegValueType(), prim_type::u32);
    EXPECT_EQ(dynamic_cast<IReg*>(reg)->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<IReg*>(reg)->GetRegType(), IReg::reg_t::v);

    EXPECT_EQ(dynamic_cast<Label*>(label)->GetLabelName(), "factorial");

    EXPECT_EQ(dynamic_cast<UInt32Const*>(uint32_const)->GetUInt32Value(), 42);

    delete reg;
    delete label;
    delete uint32_const;
}

TEST(ir_architecture, instructions) {
    InstructionBase *base_inst = new InstructionBase(class_t::base, inst_t::ret);
    InstructionBase *unary_inst = new UnaryInstr(new Label("loop"), inst_t::jmp);
    InstructionBase *binary_inst = new BinaryInstr(new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                                   new UInt32Const(42), inst_t::movi_u64);
    InstructionBase *ternary_inst = new TernaryInstr(new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                                     new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                                     new IReg(IReg::reg_t::v, 1, prim_type::u64),
                                                     inst_t::mul_u64);

    EXPECT_EQ(base_inst->GetType(), inst_t::ret);
    EXPECT_EQ(base_inst->GetClassType(), class_t::base);

    EXPECT_EQ(dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(unary_inst)->GetOpnd(0))->GetLabelName(), "loop");

    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(0))->GetRegType(), IReg::reg_t::v);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const UInt32Const *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(1))->GetUInt32Value(), 42);

    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(0))->GetRegType(), IReg::reg_t::v);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(1))->GetRegType(), IReg::reg_t::v);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(1))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(2))->GetRegType(), IReg::reg_t::v);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(2))->GetRegIdx(), 1);

    delete base_inst;
    delete unary_inst;
    delete binary_inst;
    delete ternary_inst;

}

TEST(ir_architecture, basic_block) {
    auto empty_bb = new BasicBlock();

    auto bb_pushback = new BasicBlock();
    bb_pushback->SetBBName("loop");
    auto cmp_inst = new BinaryInstr(new IReg(IReg::reg_t::v, 1, prim_type::u64),
                                    new IReg(IReg::reg_t::v, 2, prim_type::u64),
                                    inst_t::cmp_u64);
    auto ja_inst = new UnaryInstr(new Label("done"), inst_t::ja);
    auto mul_inst = new TernaryInstr(new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                              new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                              new IReg(IReg::reg_t::v, 1, prim_type::u64),
                                              inst_t::mul_u64);
    auto jmp_inst = new UnaryInstr(new Label("loop"), inst_t::jmp);

    bb_pushback->AddInstBack(cmp_inst);
    bb_pushback->AddInstBack(ja_inst);
    bb_pushback->AddInstBack(mul_inst);
    bb_pushback->AddInstBack(jmp_inst);

    cmp_inst->SetNext(ja_inst);
    ja_inst->SetPrev(cmp_inst);
    ja_inst->SetNext(mul_inst);
    mul_inst->SetPrev(ja_inst);
    mul_inst->SetNext(jmp_inst);
    jmp_inst->SetPrev(mul_inst);

    auto bb = new BasicBlock(nullptr, "loop", cmp_inst);

    EXPECT_EQ()
}


