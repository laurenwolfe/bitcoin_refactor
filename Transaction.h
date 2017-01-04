#ifndef BITCOIN_MINER_TRANSACTION_H
#define BITCOIN_MINER_TRANSACTION_H

#include <cstdint>
#include <vector>
#include <string>

#include "InputSpecifier.h"
#include "OutputSpecifier.h"
#include "BTCMiner.h"

using namespace std;

class Transaction {
    public:
        Transaction(const vector<uint8_t> &data, uint32_t &idx);

        string GetName() { return name_; };
        vector<InputSpecifier *> GetInputSpecs() { return input_specifiers_; };
        vector<OutputSpecifier *> GetOutputSpecs() { return output_specifiers_; };
        vector<uint8_t> GetSignatureDataVector() { return sig_hash_data_; };

        void ParseTransaction();
        void MakeCoinbaseTxn();

    private:
        const vector<uint8_t> &data_;
        uint32_t start_idx_, &idx_;
        uint8_t num_inputs_[SHORT_SIZE],
                num_outputs_[SHORT_SIZE];
        vector<uint8_t> name_data_, sig_hash_data_;
        vector<InputSpecifier *> input_specifiers_;
        vector<OutputSpecifier *> output_specifiers_;
        string name_;

        void BuildTxnName();
        void BuildSignatureData();
};

#endif //BITCOIN_MINER_TRANSACTION_H