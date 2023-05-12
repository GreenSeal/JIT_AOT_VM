#include <gtest/gtest.h>
#include "loop_analyzer.h"

//bool isBBInLoop(Loop *loop, BasicBlock *bb) {
//    for(size_t i = 0; i < loop->getLoopBBSize(); ++i) {
//        if(loop->getLoopBBAt(i) == bb) {
//            return true;
//        }
//    }
//    return false;
//}

TEST(loop_analyzer, without_loops) {
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

    auto bb1 = new BasicBlock(nullptr, nullptr, "A");
    auto bb2 = new BasicBlock(nullptr, nullptr, "B");
    auto bb3 = new BasicBlock(nullptr, nullptr, "C");
    auto bb4 = new BasicBlock(nullptr, nullptr, "D");
    auto bb5 = new BasicBlock(nullptr, nullptr, "E");
    auto bb6 = new BasicBlock(nullptr, nullptr, "F");
    auto bb7 = new BasicBlock(nullptr, nullptr, "G");

    IRGraph graph{nullptr};

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

    LoopAnalyzer loopAnalyzer;
    loopAnalyzer.Run(graph.GetRoot());
    Loop *rootLoop = loopAnalyzer.GetLoopTreeRoot();

    EXPECT_EQ(bb1->GetLoop(), rootLoop);
    EXPECT_EQ(bb2->GetLoop(), rootLoop);
    EXPECT_EQ(bb3->GetLoop(), rootLoop);
    EXPECT_EQ(bb4->GetLoop(), rootLoop);
    EXPECT_EQ(bb5->GetLoop(), rootLoop);
    EXPECT_EQ(bb6->GetLoop(), rootLoop);
    EXPECT_EQ(bb7->GetLoop(), rootLoop);

    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb1));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb2));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb3));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb4));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb5));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb6));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb7));
}

TEST(loop_analyzer, nested_loops) {
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

    auto bb_A = new BasicBlock(nullptr, nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, nullptr, "E");
    auto bb_F = new BasicBlock(nullptr, nullptr, "F");
    auto bb_G = new BasicBlock(nullptr, nullptr, "G");
    auto bb_H = new BasicBlock(nullptr, nullptr, "H");
    auto bb_I = new BasicBlock(nullptr, nullptr, "I");
    auto bb_J = new BasicBlock(nullptr, nullptr, "J");
    auto bb_K = new BasicBlock(nullptr, nullptr, "K");

    IRGraph graph{nullptr};

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

    LoopAnalyzer loopAnalyzer;
    loopAnalyzer.Run(graph.GetRoot());

    Loop *rootLoop = loopAnalyzer.GetLoopTreeRoot();

    EXPECT_EQ(bb_A->GetLoop(), rootLoop);
    EXPECT_EQ(bb_I->GetLoop(), rootLoop);
    EXPECT_EQ(bb_K->GetLoop(), rootLoop);
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_A));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_I));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_K));

    Loop *outerLoop = rootLoop->GetFirstSucc();
    Loop *firstInnerLoop = outerLoop->GetFirstSucc();
    Loop *secondInnerLoop = outerLoop->GetSecondSucc();

    EXPECT_EQ(bb_B->GetLoop(), outerLoop);
    EXPECT_EQ(bb_J->GetLoop(), outerLoop);
    EXPECT_EQ(bb_G->GetLoop(), outerLoop);
    EXPECT_EQ(bb_H->GetLoop(), outerLoop);
    EXPECT_EQ(outerLoop->getHeader(), bb_B);
    EXPECT_TRUE(outerLoop->isLoopContainsBB(bb_B));
    EXPECT_TRUE(outerLoop->isLoopContainsBB(bb_J));
    EXPECT_TRUE(outerLoop->isLoopContainsBB(bb_G));
    EXPECT_TRUE(outerLoop->isLoopContainsBB(bb_H));

    Loop *E_loop = bb_E->GetLoop();
    EXPECT_TRUE(((E_loop == firstInnerLoop) && (E_loop != secondInnerLoop))
    || ((E_loop == secondInnerLoop) && (E_loop != firstInnerLoop)));

    EXPECT_EQ(bb_F->GetLoop(), E_loop);
    EXPECT_EQ(E_loop->getHeader(), bb_E);
    EXPECT_TRUE(E_loop->isLoopContainsBB(bb_E));
    EXPECT_TRUE(E_loop->isLoopContainsBB(bb_F));
    EXPECT_TRUE(E_loop->isBBBackEdge(bb_F));
    EXPECT_FALSE(E_loop->isLoopContainsBB(bb_A));
    EXPECT_FALSE(E_loop->isLoopContainsBB(bb_D));

    Loop *C_loop = bb_C->GetLoop();
    EXPECT_TRUE(((C_loop == firstInnerLoop) && (C_loop != secondInnerLoop))
    || ((C_loop == secondInnerLoop) && (C_loop != firstInnerLoop)));
    EXPECT_TRUE(C_loop != E_loop);

    EXPECT_EQ(bb_D->GetLoop(), C_loop);
    EXPECT_EQ(C_loop->getHeader(), bb_C);
    EXPECT_TRUE(C_loop->isLoopContainsBB(bb_C));
    EXPECT_TRUE(C_loop->isLoopContainsBB(bb_D));
    EXPECT_TRUE(C_loop->isBBBackEdge(bb_D));
    EXPECT_FALSE(C_loop->isLoopContainsBB(bb_A));
    EXPECT_FALSE(C_loop->isLoopContainsBB(bb_F));

}

TEST(loop_analyzer, irreducible_loop) {
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

    auto bb_A = new BasicBlock(nullptr, nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, nullptr, "E");
    auto bb_F = new BasicBlock(nullptr, nullptr, "F");
    auto bb_G = new BasicBlock(nullptr, nullptr, "G");
    auto bb_H = new BasicBlock(nullptr, nullptr, "H");
    auto bb_I = new BasicBlock(nullptr, nullptr, "I");

    IRGraph ir_graph{nullptr};

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

    LoopAnalyzer loopAnalyzer;
    loopAnalyzer.Run(ir_graph.GetRoot());

    Loop *rootLoop = loopAnalyzer.GetLoopTreeRoot();

    EXPECT_EQ(bb_A->GetLoop(), rootLoop);
    EXPECT_EQ(bb_D->GetLoop(), rootLoop);
    EXPECT_EQ(bb_H->GetLoop(), rootLoop);
    EXPECT_EQ(bb_I->GetLoop(), rootLoop);
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_A));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_D));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_H));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_I));

    Loop *firstLoop = rootLoop->GetFirstSucc();
    Loop *secondLoop = rootLoop->GetSecondSucc();

    Loop *reducibleLoop, *irreducibleLoop;
    if(firstLoop->isReducible()) {
        reducibleLoop = firstLoop;
        irreducibleLoop = secondLoop;
    } else if(secondLoop->isReducible()) {
        reducibleLoop = secondLoop;
        irreducibleLoop = firstLoop;
    } else {
        ASSERT_TRUE(false);
    }

    EXPECT_EQ(bb_B->GetLoop(), reducibleLoop);
    EXPECT_EQ(bb_E->GetLoop(), reducibleLoop);
    EXPECT_EQ(bb_F->GetLoop(), reducibleLoop);
    EXPECT_EQ(reducibleLoop->getHeader(), bb_B);
    EXPECT_FALSE(reducibleLoop->isLoopContainsBB(bb_A));
    EXPECT_FALSE(reducibleLoop->isLoopContainsBB(bb_C));
    EXPECT_TRUE(reducibleLoop->isLoopContainsBB(bb_B));
    EXPECT_TRUE(reducibleLoop->isLoopContainsBB(bb_E));
    EXPECT_TRUE(reducibleLoop->isLoopContainsBB(bb_F));
    EXPECT_TRUE(reducibleLoop->isBBBackEdge(bb_F));

    EXPECT_EQ(bb_C->GetLoop(), irreducibleLoop);
    EXPECT_EQ(bb_G->GetLoop(), irreducibleLoop);
    EXPECT_EQ(irreducibleLoop->getHeader(), bb_C);
    EXPECT_FALSE(reducibleLoop->isLoopContainsBB(bb_A));
    EXPECT_FALSE(reducibleLoop->isLoopContainsBB(bb_D));
    EXPECT_TRUE(irreducibleLoop->isLoopContainsBB(bb_C));
    EXPECT_TRUE(irreducibleLoop->isLoopContainsBB(bb_G));
    EXPECT_TRUE(irreducibleLoop->isBBBackEdge(bb_G));
}

TEST(loop_analyzer, out_edge_loop) {
    /* A->B; B->C,D; C->; D->E; E->B;
     *
     *     |A|
     *      |
     *     |B|<---|
     *    /   \   |
     *  |C|   |D| |
     *         |  |
     *        |E|--
     * */

    auto bb_A = new BasicBlock(nullptr, nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, nullptr, "E");

    IRGraph graph{nullptr};

    graph.AddBBToBegin(bb_A);
    graph.AddBBInGraph(bb_B);
    graph.AddBBInGraph(bb_C);
    graph.AddBBInGraph(bb_D);
    graph.AddBBInGraph(bb_E);

    graph.AddEdge(bb_A, bb_B);
    graph.AddEdge(bb_B, bb_C);
    graph.AddEdge(bb_B, bb_D);
    graph.AddEdge(bb_D, bb_E);
    graph.AddEdge(bb_E, bb_B);

    LoopAnalyzer loopAnalyzer;
    loopAnalyzer.Run(graph.GetRoot());

    Loop *rootLoop = loopAnalyzer.GetLoopTreeRoot();

    EXPECT_EQ(bb_A->GetLoop(), rootLoop);
    EXPECT_EQ(bb_C->GetLoop(), rootLoop);
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_A));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_C));
    EXPECT_FALSE(rootLoop->isLoopContainsBB(bb_B));

    Loop *realLoop = rootLoop->GetFirstSucc();

    EXPECT_EQ(bb_B->GetLoop(), realLoop);
    EXPECT_EQ(bb_D->GetLoop(), realLoop);
    EXPECT_EQ(bb_E->GetLoop(), realLoop);
    EXPECT_EQ(realLoop->getHeader(), bb_B);
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_B));
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_D));
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_E));
    EXPECT_TRUE(realLoop->isBBBackEdge(bb_E));
    EXPECT_FALSE(realLoop->isLoopContainsBB(bb_C));
}

TEST(loop_analyzer, two_out_edge_loop) {
    /*
     * A->B; B->C; C->D,F; D->E,F; F->; E->B
     *
     *       |A|
     *        |
     *       |B|
     *      /    \
     *     /      \
     *   |C|->|D|->|E|
     *     \  /
     *      |F|
     * */

    auto bb_A = new BasicBlock(nullptr, nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, nullptr, "E");
    auto bb_F = new BasicBlock(nullptr, nullptr, "F");

    IRGraph graph{nullptr};

    graph.AddBBToBegin(bb_A);
    graph.AddBBInGraph(bb_B);
    graph.AddBBInGraph(bb_C);
    graph.AddBBInGraph(bb_D);
    graph.AddBBInGraph(bb_E);
    graph.AddBBInGraph(bb_F);

    graph.AddEdge(bb_A, bb_B);
    graph.AddEdge(bb_B, bb_C);
    graph.AddEdge(bb_C, bb_D);
    graph.AddEdge(bb_C, bb_F);
    graph.AddEdge(bb_D, bb_E);
    graph.AddEdge(bb_D, bb_F);
    graph.AddEdge(bb_E, bb_B);

    LoopAnalyzer loopAnalyzer;
    loopAnalyzer.Run(graph.GetRoot());

    Loop *rootLoop = loopAnalyzer.GetLoopTreeRoot();

    EXPECT_EQ(bb_A->GetLoop(), rootLoop);
    EXPECT_EQ(bb_F->GetLoop(), rootLoop);
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_A));
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_F));
    EXPECT_FALSE(rootLoop->isLoopContainsBB(bb_B));

    Loop *realLoop = rootLoop->GetFirstSucc();
    EXPECT_EQ(bb_B->GetLoop(), realLoop);
    EXPECT_EQ(bb_C->GetLoop(), realLoop);
    EXPECT_EQ(bb_D->GetLoop(), realLoop);
    EXPECT_EQ(bb_E->GetLoop(), realLoop);
    EXPECT_EQ(realLoop->getHeader(), bb_B);
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_B));
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_C));
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_D));
    EXPECT_TRUE(realLoop->isLoopContainsBB(bb_E));
    EXPECT_TRUE(realLoop->isBBBackEdge(bb_E));
    EXPECT_FALSE(realLoop->isLoopContainsBB(bb_F));
}

TEST(loop_analyzer, inner_outer_loop_with_if) {
    /*
     * A->B; B->C,D; C->E,H; D->E; E->F; H->; F->B,G; G->A;
     *
     *        |A|<------|
     *         |        |
     *        |B|<---|  |
     *       /   \   |  |
     *     |C|   |D| |  |
     *    /   \  /   |  |
     *  |H|   |E|    |  |
     *         |     |  |
     *        |F|-----  |
     *         |        |
     *        |G|--------
     * */

    auto bb_A = new BasicBlock(nullptr, nullptr, "A");
    auto bb_B = new BasicBlock(nullptr, nullptr, "B");
    auto bb_C = new BasicBlock(nullptr, nullptr, "C");
    auto bb_D = new BasicBlock(nullptr, nullptr, "D");
    auto bb_E = new BasicBlock(nullptr, nullptr, "E");
    auto bb_F = new BasicBlock(nullptr, nullptr, "F");
    auto bb_G = new BasicBlock(nullptr, nullptr, "G");
    auto bb_H = new BasicBlock(nullptr, nullptr, "H");

    IRGraph ir_graph{nullptr};

    ir_graph.AddBBToBegin(bb_A);
    ir_graph.AddBBInGraph(bb_B);
    ir_graph.AddBBInGraph(bb_C);
    ir_graph.AddBBInGraph(bb_D);
    ir_graph.AddBBInGraph(bb_E);
    ir_graph.AddBBInGraph(bb_F);
    ir_graph.AddBBInGraph(bb_G);
    ir_graph.AddBBInGraph(bb_H);

    ir_graph.AddEdge(bb_A, bb_B);
    ir_graph.AddEdge(bb_B, bb_C);
    ir_graph.AddEdge(bb_B, bb_D);
    ir_graph.AddEdge(bb_C, bb_E);
    ir_graph.AddEdge(bb_C, bb_H);
    ir_graph.AddEdge(bb_D, bb_E);
    ir_graph.AddEdge(bb_E, bb_F);
    ir_graph.AddEdge(bb_F, bb_G);
    ir_graph.AddEdge(bb_F, bb_B);
    ir_graph.AddEdge(bb_G, bb_A);

    LoopAnalyzer loopAnalyzer;
    loopAnalyzer.Run(ir_graph.GetRoot());

    Loop *rootLoop = loopAnalyzer.GetLoopTreeRoot();
    EXPECT_EQ(bb_H->GetLoop(), rootLoop);
    EXPECT_TRUE(rootLoop->isLoopContainsBB(bb_H));
    EXPECT_FALSE(rootLoop->isLoopContainsBB(bb_A));

    Loop *outerLoop = rootLoop->GetFirstSucc();
    Loop *innerLoop = outerLoop->GetFirstSucc();

    EXPECT_EQ(bb_A->GetLoop(), outerLoop);
    EXPECT_EQ(bb_G->GetLoop(), outerLoop);
    EXPECT_EQ(outerLoop->getHeader(), bb_A);
    EXPECT_TRUE(outerLoop->isLoopContainsBB(bb_A));
    EXPECT_TRUE(outerLoop->isLoopContainsBB(bb_G));
    EXPECT_TRUE(outerLoop->isBBBackEdge(bb_G));
    EXPECT_FALSE(outerLoop->isLoopContainsBB(bb_C));
    EXPECT_FALSE(outerLoop->isLoopContainsBB(bb_B));

    EXPECT_EQ(bb_B->GetLoop(), innerLoop);
    EXPECT_EQ(bb_C->GetLoop(), innerLoop);
    EXPECT_EQ(bb_D->GetLoop(), innerLoop);
    EXPECT_EQ(bb_E->GetLoop(), innerLoop);
    EXPECT_EQ(bb_F->GetLoop(), innerLoop);
    EXPECT_EQ(innerLoop->getHeader(), bb_B);
    EXPECT_TRUE(innerLoop->isLoopContainsBB(bb_B));
    EXPECT_TRUE(innerLoop->isLoopContainsBB(bb_C));
    EXPECT_TRUE(innerLoop->isLoopContainsBB(bb_D));
    EXPECT_TRUE(innerLoop->isLoopContainsBB(bb_E));
    EXPECT_TRUE(innerLoop->isLoopContainsBB(bb_F));
    EXPECT_TRUE(innerLoop->isBBBackEdge(bb_F));
    EXPECT_FALSE(innerLoop->isLoopContainsBB(bb_H));
    EXPECT_FALSE(innerLoop->isLoopContainsBB(bb_A));
}