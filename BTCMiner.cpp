#include <map>
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
    vector<uint8_t> binary_data;
    map<string, uint32_t> bitcoin_ledger, txn_ledger;
    uint32_t read_idx = 0, total_txns = 0, processed_txns = 0;

    class Connect *connection = new class Connect();
    connection->GetStreamBytes(binary_data);
    uint8_t empty_hash[HASH_SIZE] = {0};
    uint8_t prev_header_hash[HASH_SIZE] = {0};
    uint8_t txn_count[INT_SIZE] = {0};


    class Block *genesis = new class Block(binary_data, bitcoin_ledger,
                                           txn_ledger, read_idx, empty_hash);
    read_idx = genesis->ParseGenesisBlock(prev_header_hash);

    blockchain->AddBlock(genesis);

    read_idx += Shared::Slice(binary_data, txn_count, read_idx, INT_SIZE);

    total_txns = Shared::BinToDecimal(txn_count, INT_SIZE);

    //while(processed_txns < total_txns) {
        class Block *txn_block = new class Block(binary_data, bitcoin_ledger,
                                                txn_ledger, read_idx, prev_header_hash);

        read_idx = txn_block->ParseData(total_txns, processed_txns);
    //}

    /*
    // recipient hash, txn index
    for(auto it = txn_ledger.begin(); it != txn_ledger.end(); ++it) {
        cout << "key:value: " << it->first << " : " << it->second << endl;
    }
     */


}