#include <iostream>

#include "Transaction.h"
#include "Shared.h"
#include "Encryption.h"


Transaction::Transaction(const vector<uint8_t> &data,
                         map<string, uint32_t> &txn_ledger,
                         uint32_t txn_idx,
                         uint32_t &idx)
        : data_(data), txn_ledger_(txn_ledger), txn_idx_(txn_idx), idx_(idx) {
    name_data.reserve(HASH_SIZE);
    start_idx_ = idx;
}

bool Transaction::ParseTransaction() {
    //Get input spec count and allocate space in vector
    idx_ += Shared::Slice(data_, num_inputs_, idx_, SHORT_SIZE);
    input_specifiers_.reserve(Shared::BinToDecimal(num_inputs_, SHORT_SIZE));

    //Parse input specifiers
    for(int j = 0; j < Shared::BinToDecimal(num_inputs_, SHORT_SIZE); j++) {
        cout << "input # " << j << ": " << endl;
        class InputSpecifier *input = new class InputSpecifier(data_, idx_);
        //input->Print();
        input_specifiers_.push_back(input);
    }

    //Get output spec count and allocate space in vector
    idx_ += Shared::Slice(data_, num_outputs_, idx_, SHORT_SIZE);
    output_specifiers_.reserve(Shared::BinToDecimal(num_outputs_, SHORT_SIZE));

    for(int k = 0; k < Shared::BinToDecimal(num_outputs_, SHORT_SIZE); k++) {
        cout << "output # " << k << ": " << endl;
        class OutputSpecifier *output = new class OutputSpecifier(data_, idx_, false);
        output->Print();
        output_specifiers_.push_back(output);

        //convert binary key hash to string == key
        //txn idx corresponds to the txn where the output originated.
        txn_ledger_[output->GetHash()] = output->GetBtcAmount();
        output_sum_ += output->GetBtcAmount();
        cout << "output sum: " << output_sum_ << endl;
    }
    /*
     * todo
    // Output <= Input Bitcoin amounts
    if(num_inputs_ > 0 && (output_sum_ > input_sum_
                           || !Encryption::VerifySignature(signature, name_data))) {
        RemoveOutputs();
        return false;
    } else {
        name_ = Encryption::GenerateDHash(name_data);
    }
     */

    BuildTxnName();

    return true;
}

void Transaction::BuildTxnName() {
    vector<uint8_t> txn_slice;
    txn_slice.reserve(idx_ - start_idx_);

    for(int i = start_idx_; i < idx_; i++) {
        txn_slice.push_back(data_[i]);
    }

    name_ = Encryption::GenerateDHash(txn_slice);
}

void Transaction::MakeCoinbaseTxn() {
    class OutputSpecifier *output = new class OutputSpecifier(data_, idx_, true);
    num_inputs_[0] = 0;
    num_inputs_[1] = 0;

    num_outputs_[0] = 1;
    num_outputs_[1] = 0;


    output_specifiers_.reserve(1);
    output_specifiers_.push_back(output);
}

/*
 * todo
void Transaction::RemoveOutputs() {
    for(auto it = output_specifiers_.begin(); it != output_specifiers_.end(); ++it) {
        txn_ledger_.erase(Shared::BinToString(it->recipient_key_hash));
    }
}
 */