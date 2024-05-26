#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// BasicBlock class definition
class BasicBlock {
public:
    std::string name; // Name of the basic block
    std::vector<BasicBlock*> predecessors; // Predecessor blocks
    std::vector<BasicBlock*> successors; // Successor blocks

    // Constructor
    BasicBlock(const std::string& name) : name(name) {}

    // Add a predecessor
    void addPredecessor(BasicBlock* pred) {
        predecessors.push_back(pred);
    }

    // Add a successor
    void addSuccessor(BasicBlock* succ) {
        successors.push_back(succ);
    }
};

std::vector<BasicBlock*> computeBlocksInReversePostOrder(
        const std::vector<BasicBlock*>& blocks,
        BasicBlock* root)
{
    std::vector<BasicBlock*> postOrder;
    std::unordered_set<BasicBlock*> visited;
    std::vector<BasicBlock*> stack;

    visited.insert(root);
    stack.push_back(root);
    while (!stack.empty()) {
        auto* block = stack.back();
        bool foundNotVisited = false;
        for (auto* b : block->successors) {
            if (visited.count(b)) {
                continue;
            }
            visited.insert(b);
            stack.push_back(b);
            foundNotVisited = true;
            break;
        }

        if (!foundNotVisited) {
            postOrder.push_back(block);
            stack.pop_back();
        }
    }
    return std::vector<BasicBlock*>(postOrder.rbegin(), postOrder.rend());
}

// Function to compute the dominators
std::unordered_map<BasicBlock*, std::set<BasicBlock*>>
computeDominators(std::vector<BasicBlock*>& blocks, BasicBlock* root) {
    std::unordered_map<BasicBlock*, std::set<BasicBlock*>> dominators;

    // Initialize the dominators set for each block
    blocks = computeBlocksInReversePostOrder(blocks, root);
    for (auto block : blocks) {
        if (block == blocks[0]) { // Entry block
            dominators[block].insert(block);
        } else {
            dominators[block] = std::set<BasicBlock*>{blocks[0], block};
        }
    }

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto block : blocks) {
            if (block == blocks[0]) continue; // Skip entry block

            std::set<BasicBlock*> newDominators(blocks.begin(), blocks.end());

            for (auto pred : block->predecessors) {
                std::set<BasicBlock*> temp;
                std::set_intersection(
                    newDominators.begin(), newDominators.end(),
                    dominators[pred].begin(), dominators[pred].end(),
                    std::inserter(temp, temp.begin())
                );
                newDominators = std::move(temp);
            }
            newDominators.insert(block);

            if (newDominators != dominators[block]) {
                dominators[block] = newDominators;
                changed = true;
            }
        }
    }

    return dominators;
}

void test0() {
    std::cout << __FUNCTION__ << std::endl;

    // Create basic blocks
    BasicBlock* A = new BasicBlock("A");
    BasicBlock* B = new BasicBlock("B");
    BasicBlock* C = new BasicBlock("C");
    BasicBlock* D = new BasicBlock("D");
    BasicBlock* E = new BasicBlock("E");
    BasicBlock* F = new BasicBlock("F");
    BasicBlock* G = new BasicBlock("G");
    BasicBlock* H = new BasicBlock("H");

    // Set up CFG
    A->addSuccessor(B);
    A->addSuccessor(C);
    B->addPredecessor(A);
    C->addPredecessor(A);

    B->addSuccessor(D);
    C->addSuccessor(D);
    D->addPredecessor(B);
    D->addPredecessor(C);

    D->addSuccessor(E);
    E->addPredecessor(D);

    C->addSuccessor(E);
    E->addPredecessor(C);

    E->addSuccessor(F);
    E->addSuccessor(G);
    F->addPredecessor(E);
    G->addPredecessor(E);

    F->addSuccessor(B);
    F->addSuccessor(H);
    B->addPredecessor(F);
    H->addPredecessor(F);

    std::vector<BasicBlock*> blocks = {F, B, E, C, D, H, G, A};

    // Compute dominators
    auto dominators = computeDominators(blocks, A);

    for (auto block : blocks) {
        std::cout << block->name << "\n";
        const auto& doms = dominators[block];
        for (auto d : doms) {
            std::cout << "  " << d->name << "\n";
        }
    }

    // Clean up
    for (auto block : blocks) {
        delete block;
    }
}

void test1() {
    std::cout << __FUNCTION__ << std::endl;

    //   _A_
    //  / | \
    // B->C<-D
    //    |  |
    //    E<-F

    // Create basic blocks
    BasicBlock* A = new BasicBlock("A");
    BasicBlock* B = new BasicBlock("B");
    BasicBlock* C = new BasicBlock("C");
    BasicBlock* D = new BasicBlock("D");
    BasicBlock* E = new BasicBlock("E");
    BasicBlock* F = new BasicBlock("F");

    // Set up CFG
    A->addSuccessor(B);
    A->addSuccessor(C);
    A->addSuccessor(D);
    B->addPredecessor(A);
    C->addPredecessor(A);
    D->addPredecessor(A);

    // B->C
    B->addSuccessor(C);
    C->addPredecessor(B);

    // C<-D
    D->addSuccessor(C);
    C->addPredecessor(D);

    // C->E
    C->addSuccessor(E);
    E->addPredecessor(C);

    // D->F
    D->addSuccessor(F);
    F->addPredecessor(D);

    // F->E
    F->addSuccessor(E);
    E->addPredecessor(F);

    std::vector<BasicBlock*> blocks = {B, E, C, D, A, F};

    // Compute dominators
    auto dominators = computeDominators(blocks, A);

    for (auto block : blocks) {
        std::cout << block->name << "\n";
        const auto& doms = dominators[block];
        for (auto d : doms) {
            std::cout << "  " << d->name << "\n";
        }
    }

    // Clean up
    for (auto block : blocks) {
        delete block;
    }
}

void test2() {
    std::cout << __FUNCTION__ << std::endl;

    //   _A_
    //  / | \
    // B->C<-D
    //    |\_
    //    E<-F

    // Create basic blocks
    BasicBlock* A = new BasicBlock("A");
    BasicBlock* B = new BasicBlock("B");
    BasicBlock* C = new BasicBlock("C");
    BasicBlock* D = new BasicBlock("D");
    BasicBlock* E = new BasicBlock("E");
    BasicBlock* F = new BasicBlock("F");

    // Set up CFG
    A->addSuccessor(B);
    A->addSuccessor(C);
    A->addSuccessor(D);
    B->addPredecessor(A);
    C->addPredecessor(A);
    D->addPredecessor(A);

    // B->C
    B->addSuccessor(C);
    C->addPredecessor(B);

    // C<-D
    D->addSuccessor(C);
    C->addPredecessor(D);

    // C->E
    C->addSuccessor(E);
    E->addPredecessor(C);

    // C->F
    C->addSuccessor(F);
    F->addPredecessor(C);

    // F->E
    F->addSuccessor(E);
    E->addPredecessor(F);

    std::vector<BasicBlock*> blocks = {B, E, C, D, A, F};

    // Compute dominators
    auto dominators = computeDominators(blocks, A);

    for (auto block : blocks) {
        std::cout << block->name << "\n";
        const auto& doms = dominators[block];
        for (auto d : doms) {
            std::cout << "  " << d->name << "\n";
        }
    }

    // Clean up
    for (auto block : blocks) {
        delete block;
    }
}

int main(int argc, const char *argv[])
{
    test0();
    test1();
    test2();
    return 0;
}
