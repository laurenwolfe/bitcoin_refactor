#include <vector>
#include <string>
#include <cstdint>
#include "BTCMiner.h"

#ifndef BITCOIN_REFACTOR_INPUTSPECIFIER_H
#define BITCOIN_REFACTOR_INPUTSPECIFIER_H

class InputSpecifier {
    public:
        InputSpecifier(std::vector<uint8_t> data, uint32_t &idx);
        void Print();
        std::string GetKeyHash() { return key_hash_; };

private:
        const std::vector<uint8_t> &data_;
        uint32_t &idx_;
        uint8_t prev_txn_name_[HASH_SIZE] = {0}; //32 bytes
        uint8_t output_spec_idx_[SHORT_SIZE] = {0}; //2 bytes
        uint8_t txn_signature_[SIG_SIZE] = {0}; //128 bytes
        uint8_t key_len_[SHORT_SIZE] = {0}; //2 bytes
        std::vector<uint8_t> public_key_; //var bytes
        std::string key_hash_; // 32 bytes

        void WritePublicKey();
};

#endif //BITCOIN_REFACTOR_INPUTSPECIFIER_H