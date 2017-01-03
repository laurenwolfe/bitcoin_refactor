#ifndef BITCOIN_MINER_TRANSACTION_H
#define BITCOIN_MINER_TRANSACTION_H

#include <cstdint>
#include <vector>
#include <string>
#include <map>

#include "InputSpecifier.h"
#include "OutputSpecifier.h"
#include "BTCMiner.h"

using namespace std;

class Transaction {
    public:
        Transaction(const vector<uint8_t> &data,
                    map<string, uint32_t> &txn_ledger,
                    uint32_t txn_idx,
                    uint32_t &idx);

        const string GetName() { return name_; };
        const vector<InputSpecifier *> &GetInputSpecs() { return input_specifiers_; };
        const vector<OutputSpecifier *> &GetOutputSpecs() { return output_specifiers_; };
        bool ParseTransaction();
        void MakeCoinbaseTxn();

    private:
        const vector<uint8_t> &data_;
        map<string, uint32_t> &txn_ledger_;
        uint32_t input_sum_ = 0, output_sum_ = 0, txn_idx_, start_idx_;
        uint32_t &idx_;
        uint8_t num_inputs_[SHORT_SIZE], num_outputs_[SHORT_SIZE],
                signature[SIG_SIZE], public_key_hash_array_[HASH_SIZE],
                name_hash_array_[HASH_SIZE];
        vector<uint8_t> name_data;
        vector<InputSpecifier *> input_specifiers_;
        vector<OutputSpecifier *> output_specifiers_;
        string name_;

        void RemoveOutputs();
        void BuildTxnName();
};

#endif //BITCOIN_MINER_TRANSACTION_H