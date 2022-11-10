//
// Created by denist on 10/26/22.
//

#include <gtest/gtest.h>

#include "ir_graph.h"
#include "basic_block.h"
#include "DomTree.h"

TEST(dom_tree_algo, ir_graph1) {
    /*
     *  A->B; B->C,F; C->D; D->; E->C,D; F->E,G; G->D
     *
     *      |A|
     *       |
     *      |B|
     *     /   \
     *    /     \
     * |C|<-|E|<-|F|
     *   \  /    /
     *    |D|<-|G|
     *
     */

    auto bb1 = new BasicBlock(nullptr, "A");
    auto bb2 = new BasicBlock(nullptr, "B");
    auto bb3 = new BasicBlock(nullptr, "C");
    auto bb4 = new BasicBlock(nullptr, "D");
    auto bb5 = new BasicBlock(nullptr, "E");
    auto bb6 = new BasicBlock(nullptr, "F");
    auto bb7 = new BasicBlock(nullptr, "G");

    IRGraph graph{};

    graph.AddBBToBegin(bb1);
    graph.AddBBInGraph(bb2);
    graph.AddBBInGraph(bb3);
    graph.AddBBInGraph(bb4);
    graph.AddBBInGraph(bb5);
    graph.AddBBInGraph(bb6);
    graph.AddBBInGraph(bb7);

    graph.AddEdge(bb1, bb2);
    graph.AddEdge(bb2, bb3);
    graph.AddEdge(bb2, bb6);
    graph.AddEdge(bb6, bb5);
    graph.AddEdge(bb5, bb3);
    graph.AddEdge(bb6, bb7);
    graph.AddEdge(bb3, bb4);
    graph.AddEdge(bb5, bb4);
    graph.AddEdge(bb7, bb4);

    DomTree dom_tree_algo{};
    dom_tree_algo.RunAlgoAndBuildDomTree(&graph);

    EXPECT_TRUE(bb1->IsDominated(bb2));
    EXPECT_EQ(bb1->GetIdom(), nullptr);

    EXPECT_TRUE(bb2->IsDominated(bb3));
    EXPECT_TRUE(bb2->IsDominated(bb4));
    EXPECT_TRUE(bb2->IsDominated(bb6));
    EXPECT_EQ(bb2->GetIdom(), bb1);

    EXPECT_EQ(bb3->GetIdom(), bb2);
    EXPECT_EQ(bb4->GetIdom(), bb2);
    EXPECT_EQ(bb6->GetIdom(), bb2);

    EXPECT_TRUE(bb6->IsDominated(bb7));
    EXPECT_TRUE(bb6->IsDominated(bb5));
    EXPECT_EQ(bb7->GetIdom(), bb6);
    EXPECT_EQ(bb5->GetIdom(), bb6);

    delete bb1;
    delete bb2;
    delete bb3;
    delete bb4;
    delete bb5;
    delete bb6;
    delete bb7;
}

TEST(dom_tree_algo, ir_graph2) {
    /*
     * A->B; B->C,J; C->D; D->C,E; E->F; F->G,E; G->H,I; H->B; I->K; J->C; K->;
     *
     *         |A|
     *          |
     *   ----->|B|
     *  |       |  \
     *  |    ->|C|<-|J|
     *  |    |  |
     *  |    --|D|
     *  |       |
     *  |      |E|<-
     *  |       |  |
     *  |      |F|--
     *  |       |
     *  --|H|<-|G|->|I|
     *               |
     *              |K|
     */

    auto bb_A = new BasicBlock(nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, "E");
    auto bb_F = new BasicBlock(nullptr, "F");
    auto bb_G = new BasicBlock(nullptr, "G");
    auto bb_H = new BasicBlock(nullptr, "H");
    auto bb_I = new BasicBlock(nullptr, "I");
    auto bb_J = new BasicBlock(nullptr, "J");
    auto bb_K = new BasicBlock(nullptr, "K");

    IRGraph graph{};

    graph.AddBBToBegin(bb_A);
    graph.AddBBInGraph(bb_B);
    graph.AddBBInGraph(bb_C);
    graph.AddBBInGraph(bb_D);
    graph.AddBBInGraph(bb_E);
    graph.AddBBInGraph(bb_F);
    graph.AddBBInGraph(bb_G);
    graph.AddBBInGraph(bb_H);
    graph.AddBBInGraph(bb_I);
    graph.AddBBInGraph(bb_J);
    graph.AddBBInGraph(bb_K);

    graph.AddEdge(bb_A, bb_B);
    graph.AddEdge(bb_B, bb_C);
    graph.AddEdge(bb_B, bb_J);
    graph.AddEdge(bb_C, bb_D);
    graph.AddEdge(bb_D, bb_C);
    graph.AddEdge(bb_D, bb_E);
    graph.AddEdge(bb_E, bb_F);
    graph.AddEdge(bb_F, bb_E);
    graph.AddEdge(bb_F, bb_G);
    graph.AddEdge(bb_G, bb_H);
    graph.AddEdge(bb_G, bb_I);
    graph.AddEdge(bb_H, bb_B);
    graph.AddEdge(bb_I, bb_K);
    graph.AddEdge(bb_J, bb_C);

    DomTree dom_tree_algo{};
    dom_tree_algo.RunAlgoAndBuildDomTree(&graph);

    EXPECT_TRUE(bb_A->IsDominated(bb_B));
    EXPECT_EQ(bb_A->GetIdom(), nullptr);

    EXPECT_TRUE(bb_B->IsDominated(bb_C));
    EXPECT_TRUE(bb_B->IsDominated(bb_J));
    EXPECT_EQ(bb_B->GetIdom(), bb_A);

    EXPECT_TRUE(bb_C->IsDominated(bb_D));
    EXPECT_EQ(bb_C->GetIdom(), bb_B);

    EXPECT_TRUE(bb_D->IsDominated(bb_E));
    EXPECT_EQ(bb_D->GetIdom(), bb_C);

    EXPECT_TRUE(bb_E->IsDominated(bb_F));
    EXPECT_EQ(bb_E->GetIdom(), bb_D);

    EXPECT_TRUE(bb_F->IsDominated(bb_G));
    EXPECT_EQ(bb_F->GetIdom(), bb_E);

    EXPECT_TRUE(bb_G->IsDominated(bb_H));
    EXPECT_TRUE(bb_G->IsDominated(bb_I));
    EXPECT_EQ(bb_G->GetIdom(), bb_F);

    EXPECT_EQ(bb_H->GetIdom(), bb_G);

    EXPECT_TRUE(bb_I->IsDominated(bb_K));
    EXPECT_EQ(bb_I->GetIdom(), bb_G);

    EXPECT_EQ(bb_K->GetIdom(), bb_I);
}

TEST(dom_tree_algo, ir_graph3) {
    /*
     *  A->B; B->E,C; C->D; D->G; E->F,D; F->H,B; G->I,C; H->G,I; I->;
     *
     *      |A|
     *       |
     *  --->|B|
     * |   /  \
     * |  |E| |C|<--
     * |   | \ |    |
     * ---|F| |D|   |
     *     |    \   |
     *    |H|->|G|---
     *      \  /
     *       |I|
     *
     */

    auto bb_A = new BasicBlock(nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, "E");
    auto bb_F = new BasicBlock(nullptr, "F");
    auto bb_G = new BasicBlock(nullptr, "G");
    auto bb_H = new BasicBlock(nullptr, "H");
    auto bb_I = new BasicBlock(nullptr, "I");

    IRGraph ir_graph{};

    ir_graph.AddBBToBegin(bb_A);
    ir_graph.AddBBInGraph(bb_B);
    ir_graph.AddBBInGraph(bb_C);
    ir_graph.AddBBInGraph(bb_D);
    ir_graph.AddBBInGraph(bb_E);
    ir_graph.AddBBInGraph(bb_F);
    ir_graph.AddBBInGraph(bb_G);
    ir_graph.AddBBInGraph(bb_H);
    ir_graph.AddBBInGraph(bb_I);

    ir_graph.AddEdge(bb_A, bb_B);
    ir_graph.AddEdge(bb_B, bb_C);
    ir_graph.AddEdge(bb_B, bb_E);
    ir_graph.AddEdge(bb_C, bb_D);
    ir_graph.AddEdge(bb_D, bb_G);
    ir_graph.AddEdge(bb_E, bb_F);
    ir_graph.AddEdge(bb_E, bb_D);
    ir_graph.AddEdge(bb_F, bb_H);
    ir_graph.AddEdge(bb_F, bb_B);
    ir_graph.AddEdge(bb_G, bb_I);
    ir_graph.AddEdge(bb_G, bb_C);
    ir_graph.AddEdge(bb_H, bb_G);
    ir_graph.AddEdge(bb_H, bb_I);

    DomTree dom_tree_algo{};
    dom_tree_algo.RunAlgoAndBuildDomTree(&ir_graph);

    EXPECT_TRUE(bb_A->IsDominated(bb_B));
    EXPECT_EQ(bb_A->GetIdom(), nullptr);

    EXPECT_TRUE(bb_B->IsDominated(bb_C));
    EXPECT_TRUE(bb_B->IsDominated(bb_D));
    EXPECT_TRUE(bb_B->IsDominated(bb_E));
    EXPECT_TRUE(bb_B->IsDominated(bb_G));
    EXPECT_TRUE(bb_B->IsDominated(bb_I));
    EXPECT_EQ(bb_B->GetIdom(), bb_A);

    EXPECT_EQ(bb_C->GetIdom(), bb_B);
    EXPECT_EQ(bb_D->GetIdom(), bb_B);
    EXPECT_EQ(bb_G->GetIdom(), bb_B);

    EXPECT_TRUE(bb_E->IsDominated(bb_F));
    EXPECT_EQ(bb_E->GetIdom(), bb_B);

    EXPECT_TRUE(bb_F->IsDominated(bb_H));
    EXPECT_EQ(bb_F->GetIdom(), bb_E);

    EXPECT_EQ(bb_H->GetIdom(), bb_F);
    EXPECT_EQ(bb_I->GetIdom(), bb_B);
}