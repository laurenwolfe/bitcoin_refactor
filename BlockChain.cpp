#include "BlockChain.h"

using namespace std;

BlockChain::BlockChain() {
    blocks_.reserve(1);
}


bool BlockChain::AddBlock(Block *block) {
    blocks_.push_back(block);
    size_++;
    return true;
}


//TODO
bool BlockChain::WriteBinaryToFile() {
    return false;
}