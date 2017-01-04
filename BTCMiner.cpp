#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <cstdbool>

#include "BTCMiner.h"
#include "Connect.h"
#include "BlockChain.h"
#include "Block.h"
#include "Shared.h"

using namespace std;

int main(int argc, char *argv[]) {
    class BlockChain *blockchain = new class BlockChain();
    uint32_t read_idx = 0, total_txns = 1, processed_txns = 0;

    vector<uint8_t> binary_data;
    unordered_map<string, Transaction> txns;
    unordered_map<string, set<uint32_t>> unused_outputs;
    unordered_map<string, uint32_t> bitcoin_ledger;

    class Connect *connection = new class Connect();
    connection->GetStreamBytes(binary_data);
    uint8_t empty_hash[HASH_SIZE] = {0};
    uint8_t prev_header_hash[HASH_SIZE] = {0};
    uint8_t txn_count[INT_SIZE] = {0};


    class Block *genesis = new class Block(binary_data,
                                           txns,
                                           unused_outputs,
                                           bitcoin_ledger,
                                           read_idx,
                                           total_txns,
                                           processed_txns,
                                           empty_hash);
    read_idx = genesis->ParseGenesisBlock();

    blockchain->AddBlock(genesis);

    read_idx += Shared::Slice(binary_data, txn_count, read_idx, INT_SIZE);

    total_txns = Shared::BinToDecimal(txn_count, INT_SIZE);

    while(processed_txns < total_txns) {
        class Block *txn_block = new class Block(binary_data,
                                                 txns,
                                                 unused_outputs,
                                                 bitcoin_ledger,
                                                 read_idx,
                                                 total_txns,
                                                 processed_txns,
                                                 prev_header_hash);

        read_idx = txn_block->ParseData();

        /* todo: activate
        if(txn_block->ConstructHeader()) {
            blockchain->AddBlock(txn_block);
        }
        */

        //todo: output binary file
        //todo: print bitcoin ledger
    }
}