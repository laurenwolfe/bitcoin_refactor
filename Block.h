#ifndef BTC_MINER_BLOCK_H
#define BTC_MINER_BLOCK_H

#include <vector>
#include <cstdint>
#include <string>
#include <map>
#include <cstdbool>

#include "Transaction.h"

using namespace std;

class Block {
    public:
        Block(const vector<uint8_t> &data,
              map<string, uint32_t> &bitcoin_ledger,
              map<string, uint32_t> &txn_ledger,
              uint32_t &idx,
              uint8_t prev_header_hash[HASH_SIZE]);

        uint32_t ParseGenesisBlock(uint8_t *prev_header_hash_);
        uint32_t ParseData(uint32_t total_txns, uint32_t &processed_txns);

    private:
        const vector<uint8_t> &data_;
        map<string, uint32_t> &bitcoin_ledger_, &txn_ledger_;
        uint32_t idx_, start_idx_, processed_txns_;
        uint8_t header_[HEADER_SIZE] = {0},
                merkle_root_[HASH_SIZE] = {0},
                prev_header_hash_[HASH_SIZE] = {0},
                header_hash_[HASH_SIZE] = {0},
                txn_count_[INT_SIZE] = {0};
        vector<Transaction *> txns_;
        vector<uint8_t *> txn_hashes_;
        string block_name_;

        void MakeCoinbaseTxn();
        bool CalculateMerkleRoot();
        bool ConstructHeader();
        void ConcatVector(vector<uint8_t> left, vector<uint8_t> right);
        bool FindNonce(vector<uint8_t> header_);
        bool HashHeader();
        bool InsertTxn(Transaction txn);
        void GetUnixTime(vector<unsigned char > &time_vector);
        bool GetTxns(uint32_t count, uint32_t &processed_txns);
};

#endif //BTC_MINER_BLOCK_H