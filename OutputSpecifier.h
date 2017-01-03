#include <vector>
#include <string>
#include <cstdint>
#include "BTCMiner.h"
#include "Shared.h"

#ifndef BITCOIN_REFACTOR_OUTPUTSPECIFIER_H
#define BITCOIN_REFACTOR_OUTPUTSPECIFIER_H

class OutputSpecifier {
    public:
        OutputSpecifier(std::vector<uint8_t> data, uint32_t &idx, bool is_coinbase);
        void Print();

        uint32_t GetBtcAmount() { return Shared::BinToDecimal(btc_amount_, INT_SIZE); };
        std::string GetHash() { return Shared::BinToString(recipient_key_hash_, HASH_SIZE); };

    private:
        const std::vector<uint8_t> &data_;
        uint32_t &idx_;
        uint8_t btc_amount_[INT_SIZE] = {0}; //4 bytes
        uint8_t recipient_key_hash_[HASH_SIZE] = {0}; //32 bytes
};

#endif //BITCOIN_REFACTOR_OUTPUTSPECIFIER_H