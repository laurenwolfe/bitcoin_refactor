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
                    map<string, vector<bool>> &txn_ledger,
                    uint32_t txn_idx,
                    uint32_t &idx);
        const string GetName() { return name_; };
        uint8_t *GetNameHashArray() { return name_hash_array_; };
        const vector<InputSpecifier *> &GetInputSpecs() { return input_specifiers_; };
        const vector<OutputSpecifier *> &GetOutputSpecs() { return output_specifiers_; };
        vector<uint8_t> GetSignatureDataVector() { return sig_hash_data_; };
        string GetSignatureDataHash() { return signature_hash_; };
        void ParseTransaction();
        void MakeCoinbaseTxn();

    private:
        const vector<uint8_t> &data_;
        map<string, vector<bool>> &txn_ledger_;
        uint32_t txn_idx_, start_idx_;
        uint32_t &idx_;
        uint8_t num_inputs_[SHORT_SIZE],
                num_outputs_[SHORT_SIZE],
                name_hash_array_[HASH_SIZE];
        vector<uint8_t> name_data_, sig_hash_data_;
        vector<InputSpecifier *> input_specifiers_;
        vector<OutputSpecifier *> output_specifiers_;
        string name_, signature_hash_;

        void RemoveOutputs();
        void BuildTxnName();
        void BuildTxnSignatureHash();
};

#endif //BITCOIN_MINER_TRANSACTION_H