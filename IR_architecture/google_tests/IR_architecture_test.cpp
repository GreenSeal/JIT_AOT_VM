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

    auto reg_clone = reg->clone();
    auto label_clone = label->clone();
    auto uint32_const_clone = uint32_const->clone();

    EXPECT_EQ(dynamic_cast<IReg*>(reg_clone)->GetRegValueType(),
              dynamic_cast<IReg*>(reg)->GetRegValueType());
    EXPECT_EQ(dynamic_cast<IReg*>(reg_clone)->GetRegIdx(),
              dynamic_cast<IReg*>(reg)->GetRegIdx());
    EXPECT_EQ(dynamic_cast<IReg*>(reg_clone)->GetRegType(),
              dynamic_cast<IReg*>(reg)->GetRegType());

    EXPECT_EQ(dynamic_cast<Label*>(label_clone)->GetLabelName(),
              dynamic_cast<Label*>(label)->GetLabelName());

    EXPECT_EQ(dynamic_cast<UInt32Const*>(uint32_const_clone)->GetUInt32Value(),
              dynamic_cast<UInt32Const*>(uint32_const)->GetUInt32Value());

    delete reg;
    delete label;
    delete uint32_const;
    delete reg_clone;
    delete label_clone;
    delete uint32_const_clone;
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

    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const UInt32Const *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(1))->GetUInt32Value(), 42);

    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(1))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(2))->GetRegIdx(), 1);

    auto unary_inst_clone = unary_inst->clone();
    auto binary_inst_clone = binary_inst->clone();
    auto ternary_inst_clone = ternary_inst->clone();

    EXPECT_EQ(dynamic_cast<UnaryInstr *>(unary_inst)->GetType(), dynamic_cast<UnaryInstr *>(unary_inst_clone)->GetType());
    EXPECT_EQ(dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(unary_inst)->GetOpnd(0))->GetLabelName(),
              dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(unary_inst_clone)->GetOpnd(0))->GetLabelName());

    EXPECT_EQ(dynamic_cast<BinaryInstr *>(binary_inst)->GetType(), dynamic_cast<BinaryInstr *>(binary_inst_clone)->GetType());
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(0))->GetRegIdx(),
              dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(binary_inst_clone)->GetOpnd(0))->GetRegIdx());
    EXPECT_EQ(dynamic_cast<const UInt32Const *>(dynamic_cast<BinaryInstr *>(binary_inst)->GetOpnd(1))->GetUInt32Value(),
              dynamic_cast<const UInt32Const *>(dynamic_cast<BinaryInstr *>(binary_inst_clone)->GetOpnd(1))->GetUInt32Value());

    EXPECT_EQ(dynamic_cast<TernaryInstr *>(ternary_inst)->GetType(), dynamic_cast<TernaryInstr *>(ternary_inst_clone)->GetType());
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(0))->GetRegIdx(),
              dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst_clone)->GetOpnd(0))->GetRegIdx());
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(1))->GetRegIdx(),
              dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst_clone)->GetOpnd(1))->GetRegIdx());
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst)->GetOpnd(2))->GetRegIdx(),
              dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(ternary_inst_clone)->GetOpnd(2))->GetRegIdx());

    delete base_inst;
    delete unary_inst;
    delete binary_inst;
    delete ternary_inst;
    delete unary_inst_clone;
    delete binary_inst_clone;
    delete ternary_inst_clone;

}

TEST(ir_architecture, basic_block) {
    auto empty_bb = new BasicBlock();
    EXPECT_EQ(empty_bb->begin(), empty_bb->end());

    auto cmp_inst = new BinaryInstr(new IReg(IReg::reg_t::v, 1, prim_type::u64),
                                    new IReg(IReg::reg_t::v, 2, prim_type::u64),
                                    inst_t::cmp_u64);
    auto ja_inst = new UnaryInstr(new Label("done"), inst_t::ja);
    auto mul_inst = new TernaryInstr(new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                              new IReg(IReg::reg_t::v, 0, prim_type::u64),
                                              new IReg(IReg::reg_t::v, 1, prim_type::u64),
                                              inst_t::mul_u64);
    auto jmp_inst = new UnaryInstr(new Label("loop"), inst_t::jmp);

    auto bb_pushback = new BasicBlock();
    bb_pushback->SetBBName("loop");

    EXPECT_EQ(bb_pushback->GetBBName(), "loop");

    bb_pushback->AddInstBack(cmp_inst);
    bb_pushback->AddInstBack(ja_inst);
    bb_pushback->AddInstBack(mul_inst);
    bb_pushback->AddInstBack(jmp_inst);

    auto bb_pb_it = bb_pushback->begin();
    EXPECT_EQ(dynamic_cast<BinaryInstr *>(*bb_pb_it)->GetType(), inst_t::cmp_u64);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(*bb_pb_it)->GetOpnd(0))->GetRegIdx(),
              1);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(*bb_pb_it)->GetOpnd(1))->GetRegIdx(),
              2);

    ++bb_pb_it;
    EXPECT_EQ(dynamic_cast<UnaryInstr *>(*bb_pb_it)->GetType(), inst_t::ja);
    EXPECT_EQ(dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(*bb_pb_it)->GetOpnd(0))->GetLabelName(),
              "done");

    ++bb_pb_it;
    EXPECT_EQ(dynamic_cast<TernaryInstr *>(*bb_pb_it)->GetType(), inst_t::mul_u64);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(*bb_pb_it)->GetOpnd(0))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(*bb_pb_it)->GetOpnd(1))->GetRegIdx(), 0);
    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(*bb_pb_it)->GetOpnd(2))->GetRegIdx(), 1);

    ++bb_pb_it;
    EXPECT_EQ(dynamic_cast<UnaryInstr *>(*bb_pb_it)->GetType(), inst_t::jmp);
    EXPECT_EQ(dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(*bb_pb_it)->GetOpnd(0))->GetLabelName(), "loop");

    ++bb_pb_it;
    EXPECT_EQ(bb_pb_it, bb_pushback->end());

    cmp_inst->SetNext(ja_inst);
    ja_inst->SetPrev(cmp_inst);
    ja_inst->SetNext(mul_inst);
    mul_inst->SetPrev(ja_inst);
    mul_inst->SetNext(jmp_inst);
    jmp_inst->SetPrev(mul_inst);

    auto bb = new BasicBlock(nullptr, "loop", cmp_inst);

    auto bb_it = bb->begin();
    EXPECT_EQ(*bb_it, nullptr);
//    EXPECT_EQ(dynamic_cast<BinaryInstr *>(*bb_it)->GetType(), inst_t::cmp_u64);
//    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(*bb_it)->GetOpnd(0))->GetRegIdx(),
//              1);
//    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<BinaryInstr *>(*bb_it)->GetOpnd(1))->GetRegIdx(),
//              2);
//
//    ++bb_it;
//    EXPECT_EQ(dynamic_cast<UnaryInstr *>(*bb_it)->GetType(), inst_t::ja);
//    EXPECT_EQ(dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(*bb_it)->GetOpnd(0))->GetLabelName(),
//              "done");
//
//    ++bb_it;
//    EXPECT_EQ(dynamic_cast<TernaryInstr *>(*bb_it)->GetType(), inst_t::mul_u64);
//    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(*bb_it)->GetOpnd(0))->GetRegIdx(), 0);
//    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(*bb_it)->GetOpnd(1))->GetRegIdx(), 0);
//    EXPECT_EQ(dynamic_cast<const IReg *>(dynamic_cast<TernaryInstr *>(*bb_it)->GetOpnd(2))->GetRegIdx(), 1);
//
//    ++bb_it;
//    EXPECT_EQ(dynamic_cast<UnaryInstr *>(*bb_it)->GetType(), inst_t::jmp);
//    EXPECT_EQ(dynamic_cast<const Label *>(dynamic_cast<UnaryInstr *>(*bb_it)->GetOpnd(0))->GetLabelName(), "loop");
//
//    ++bb_it;
//    EXPECT_EQ(bb_it, bb->end());

    delete empty_bb;
    delete bb_pushback;
    delete bb;
    delete cmp_inst;
    delete ja_inst;
    delete mul_inst;
    delete jmp_inst;
}


