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
     *       /
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

    bb1->AddSucc(bb2);
    bb2->AddPredec(bb1);

    bb2->AddSucc(bb3);
    bb3->AddPredec(bb2);
    bb2->AddSucc(bb6);
    bb6->AddPredec(bb2);

    bb3->AddSucc(bb4);
    bb4->AddPredec(bb3);

    bb5->AddSucc(bb4);
    bb4->AddPredec(bb5);
    bb5->AddSucc(bb3);
    bb3->AddPredec(bb5);

    bb6->AddSucc(bb5);
    bb5->AddPredec(bb6);
    bb6->AddSucc(bb7);
    bb7->AddPredec(bb6);

    bb7->AddSucc(bb4);
    bb4->AddPredec(bb7);

    IRGraph graph(bb1);
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
}