#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// BasicBlock class definition
class BasicBlock {
 public:
  std::string name;  // Name of the basic block
  std::vector<std::weak_ptr<BasicBlock>> predecessors;  // Predecessor blocks
  std::vector<std::weak_ptr<BasicBlock>> successors;    // Successor blocks

  // Constructor
  BasicBlock(const std::string& name) : name(name) {}

  // Add a predecessor
  void addPredecessor(std::shared_ptr<BasicBlock> pred) {
    predecessors.push_back(pred);
  }

  // Add a successor
  void addSuccessor(std::shared_ptr<BasicBlock> succ) {
    successors.push_back(succ);
  }

  ~BasicBlock() { std::cout << "Clean up " << name << std::endl; }
};

std::vector<std::shared_ptr<BasicBlock>> computeBlocksInReversePostOrder(
    const std::vector<std::shared_ptr<BasicBlock>>& blocks,
    std::shared_ptr<BasicBlock> root) {
  std::vector<std::shared_ptr<BasicBlock>> postOrder;
  std::unordered_set<std::shared_ptr<BasicBlock>> visited;
  std::vector<std::shared_ptr<BasicBlock>> stack;

  visited.insert(root);
  stack.push_back(root);
  while (!stack.empty()) {
    auto block = stack.back();
    bool foundNotVisited = false;
    for (auto b : block->successors) {
      auto pb = b.lock();
      if (visited.count(pb)) {
        continue;
      }
      visited.insert(pb);
      stack.push_back(pb);
      foundNotVisited = true;
      break;
    }

    if (!foundNotVisited) {
      postOrder.push_back(block);
      stack.pop_back();
    }
  }
  return std::vector<std::shared_ptr<BasicBlock>>(postOrder.rbegin(),
                                                  postOrder.rend());
}

// Function to compute the dominators
std::unordered_map<std::shared_ptr<BasicBlock>,
                   std::set<std::shared_ptr<BasicBlock>>>
computeDominators(std::vector<std::shared_ptr<BasicBlock>>& blocks,
                  std::shared_ptr<BasicBlock> root) {
  std::unordered_map<std::shared_ptr<BasicBlock>,
                     std::set<std::shared_ptr<BasicBlock>>>
      dominators;

  // Initialize the dominators set for each block
  blocks = computeBlocksInReversePostOrder(blocks, root);
  for (auto block : blocks) {
    if (block == blocks[0]) {  // Entry block
      dominators[block].insert(block);
    } else {
      dominators[block] =
          std::set<std::shared_ptr<BasicBlock>>{blocks[0], block};
    }
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (auto block : blocks) {
      if (block == blocks[0]) continue;  // Skip entry block

      std::set<std::shared_ptr<BasicBlock>> newDominators(blocks.begin(),
                                                          blocks.end());

      for (auto pred : block->predecessors) {
        auto ppred = pred.lock();
        std::set<std::shared_ptr<BasicBlock>> temp;
        std::set_intersection(newDominators.begin(), newDominators.end(),
                              dominators[ppred].begin(),
                              dominators[ppred].end(),
                              std::inserter(temp, temp.begin()));
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
  auto A = std::make_shared<BasicBlock>("A");
  auto B = std::make_shared<BasicBlock>("B");
  auto C = std::make_shared<BasicBlock>("C");
  auto D = std::make_shared<BasicBlock>("D");
  auto E = std::make_shared<BasicBlock>("E");
  auto F = std::make_shared<BasicBlock>("F");
  auto G = std::make_shared<BasicBlock>("G");
  auto H = std::make_shared<BasicBlock>("H");

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

  std::vector<std::shared_ptr<BasicBlock>> blocks = {F, B, E, C, D, H, G, A};

  // Compute dominators
  auto dominators = computeDominators(blocks, A);

  for (auto block : blocks) {
    std::cout << block->name << "\n";
    const auto& doms = dominators[block];
    for (auto d : doms) {
      std::cout << "  " << d->name << "\n";
    }
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
  std::shared_ptr<BasicBlock> A = std::make_shared<BasicBlock>("A");
  std::shared_ptr<BasicBlock> B = std::make_shared<BasicBlock>("B");
  std::shared_ptr<BasicBlock> C = std::make_shared<BasicBlock>("C");
  std::shared_ptr<BasicBlock> D = std::make_shared<BasicBlock>("D");
  std::shared_ptr<BasicBlock> E = std::make_shared<BasicBlock>("E");
  std::shared_ptr<BasicBlock> F = std::make_shared<BasicBlock>("F");

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

  std::vector<std::shared_ptr<BasicBlock>> blocks = {B, E, C, D, A, F};

  // Compute dominators
  auto dominators = computeDominators(blocks, A);

  for (auto block : blocks) {
    std::cout << block->name << "\n";
    const auto& doms = dominators[block];
    for (auto d : doms) {
      std::cout << "  " << d->name << "\n";
    }
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
  std::shared_ptr<BasicBlock> A = std::make_shared<BasicBlock>("A");
  std::shared_ptr<BasicBlock> B = std::make_shared<BasicBlock>("B");
  std::shared_ptr<BasicBlock> C = std::make_shared<BasicBlock>("C");
  std::shared_ptr<BasicBlock> D = std::make_shared<BasicBlock>("D");
  std::shared_ptr<BasicBlock> E = std::make_shared<BasicBlock>("E");
  std::shared_ptr<BasicBlock> F = std::make_shared<BasicBlock>("F");

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

  std::vector<std::shared_ptr<BasicBlock>> blocks = {B, E, C, D, A, F};

  // Compute dominators
  auto dominators = computeDominators(blocks, A);

  for (auto block : blocks) {
    std::cout << block->name << "\n";
    const auto& doms = dominators[block];
    for (auto d : doms) {
      std::cout << "  " << d->name << "\n";
    }
  }
}

int main(int argc, const char* argv[]) {
  test0();
  test1();
  test2();
  return 0;
}
