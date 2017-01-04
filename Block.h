#ifndef BTC_MINER_BLOCK_H
#define BTC_MINER_BLOCK_H

#include <vector>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <set>
#include <cstdbool>

#include "Transaction.h"

using namespace std;

class Block {
    public:
        Block(const vector<uint8_t> &data,
              unordered_map<string, Transaction *> &txns,
              unordered_map<string, set<uint32_t>> &unused_outputs,
              unordered_map<string, uint32_t> &bitcoin_ledger,
              uint32_t &idx,
              uint32_t total_txns,
              uint32_t &processed_txns,
              uint8_t *prev_header_hash);

        uint32_t ParseGenesisBlock();
        uint32_t ParseData();

    private:
        const vector<uint8_t> &data_;
        unordered_map<string, Transaction *> &txns_;
        unordered_map<string, set<uint32_t>> &unused_outputs_;
        unordered_map<string, uint32_t> &bitcoin_ledger_;
        uint32_t &idx_, start_idx_, total_txns_, &processed_txns_;
        vector<string> block_txn_keys_;
        uint8_t prev_header_hash_[HASH_SIZE] = {0},
                header_[HEADER_SIZE] = {0},
                merkle_root_[HASH_SIZE] = {0},
                header_hash_[HASH_SIZE] = {0};
        string block_name_;
        string my_hash_ = "1f5a0200bc94ae4264642855786d9c2bb436b9e129ef95e6416136c03f339581";

        bool GetTxns(bool is_genesis);
        Transaction *MakeCoinbaseTxn();
        bool CalculateMerkleRoot();
        bool ConstructHeader();
        void GetUnixTime(vector<unsigned char > &time_vector);
        bool FindNonce(const vector<uint8_t> &header_);
};

#endif //BTC_MINER_BLOCK_H