#include <iostream>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
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

std::vector<BasicBlock*> computeBlocksInPreOrder(
        const std::vector<BasicBlock*>& blocks,
        BasicBlock* root)
{
    std::vector<BasicBlock*> preOrder;
    std::unordered_set<BasicBlock*> visited;
    std::queue<BasicBlock*> blocksToVisit;
    blocksToVisit.push(root);
    while (visited.size() != blocks.size()) {
        auto* block = blocksToVisit.front();
        blocksToVisit.pop();

        if (visited.count(block)) continue;
        visited.insert(block);
        preOrder.push_back(block);

        for (auto* b : block->successors) {
            blocksToVisit.push(b);
        }
    }
    return preOrder;
}

// Function to compute the dominators
std::unordered_map<BasicBlock*, std::set<BasicBlock*>>
computeDominators(std::vector<BasicBlock*>& blocks, BasicBlock* root) {
    std::unordered_map<BasicBlock*, std::set<BasicBlock*>> dominators;

    // Initialize the dominators set for each block
    blocks = computeBlocksInPreOrder(blocks, root);
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

int main() {
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

    return 0;
}
