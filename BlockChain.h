//
// Created by Lauren Wolfe on 12/27/16.
//

#ifndef BTC_MINER_BLOCKCHAIN_H
#define BTC_MINER_BLOCKCHAIN_H

#include "Block.h"

using namespace std;

class BlockChain {
    public:
        BlockChain();

        int size() { return size_; };

        bool AddBlock(class Block *block);
        bool WriteBinaryToFile();

    private:
        vector<Block *> blocks_;
        int size_ = 0;
};

#endif //BTC_MINER_BLOCKCHAIN_H
