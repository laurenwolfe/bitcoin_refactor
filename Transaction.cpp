#include <iostream>

#include "Transaction.h"
#include "Shared.h"
#include "Encryption.h"


Transaction::Transaction(const vector<uint8_t> &data, uint32_t &idx)
        : data_(data), idx_(idx) {
    name_data_.reserve(HASH_SIZE);
    sig_hash_data_.reserve(HASH_SIZE);
    start_idx_ = idx;
}

void Transaction::ParseTransaction() {
    //Get input spec count and allocate space in vector
    idx_ += Shared::Slice(data_, num_inputs_, idx_, SHORT_SIZE);
    input_specifiers_.reserve(Shared::BinToDecimal(num_inputs_, SHORT_SIZE));

    uint32_t num_in = Shared::BinToDecimal(num_inputs_, SHORT_SIZE);

    //Parse input specifiers
    for(int j = 0; j < num_in; j++) {
        class InputSpecifier *input = new class InputSpecifier(data_, idx_);

        //cout << "input # " << j << ": " << endl;
        //input->Print();

        input_specifiers_.push_back(input);
    }

    //Get output spec count and allocate space in vector
    idx_ += Shared::Slice(data_, num_outputs_, idx_, SHORT_SIZE);

    uint32_t output_count = Shared::BinToDecimal(num_outputs_, SHORT_SIZE);
    output_specifiers_.reserve(output_count);

    for(int k = 0; k < output_count; k++) {
        class OutputSpecifier *output = new class OutputSpecifier(data_, idx_, false);

        //cout << "output # " << k << ": " << endl;
        //output->Print();

        output_specifiers_.push_back(output);
    }

    BuildTxnName();
    BuildSignatureData();
}

void Transaction::BuildTxnName() {
    vector<uint8_t> txn_slice;
    txn_slice.reserve(idx_ - start_idx_);

    for(int i = start_idx_; i < idx_; i++) {
        txn_slice.push_back(data_[i]);
    }

    name_ = Encryption::GenerateDHash(txn_slice);
}

//Create byte vector of transaction, eliding signatures
void Transaction::BuildSignatureData() {
    Shared::PushArrayToVector(num_inputs_, SHORT_SIZE, sig_hash_data_);

    for(int i = 0; i < input_specifiers_.size(); i++) {
        Shared::PushArrayToVector(input_specifiers_[i]->GetPrevTxnNameArr(),
                                  HASH_SIZE, sig_hash_data_);
        Shared::PushArrayToVector(input_specifiers_[i]->GetOutputIdxArr(),
                                  SHORT_SIZE, sig_hash_data_);
        Shared::PushArrayToVector(input_specifiers_[i]->GetKeyLenArr(),
                                  SHORT_SIZE, sig_hash_data_);
        sig_hash_data_.insert(sig_hash_data_.end(),
                              input_specifiers_[i]->GetPublicKeyVector().begin(),
                              input_specifiers_[i]->GetPublicKeyVector().end());
    }

    Shared::PushArrayToVector(num_outputs_, SHORT_SIZE, sig_hash_data_);

    for(int i = 0; i < output_specifiers_.size(); i++) {
        Shared::PushArrayToVector(output_specifiers_[i]->GetBtcAmountArr(), INT_SIZE, sig_hash_data_);
        Shared::PushArrayToVector(output_specifiers_[i]->GetHashArr(), HASH_SIZE, sig_hash_data_);
    }
}

void Transaction::MakeCoinbaseTxn() {
    //size of txn
    name_data_.reserve(40);

    class OutputSpecifier *output = new class OutputSpecifier(data_, idx_, true);
    num_inputs_[0] = 0;
    num_inputs_[1] = 0;
    num_outputs_[0] = 1;
    num_outputs_[1] = 0;

    output_specifiers_.reserve(1);
    output_specifiers_.push_back(output);

    name_data_.push_back(num_inputs_[0]);
    name_data_.push_back(num_inputs_[1]);
    name_data_.push_back(num_outputs_[0]);
    name_data_.push_back(num_outputs_[1]);

    Shared::PushArrayToVector(output->GetBtcAmountArr(), INT_SIZE, name_data_);
    Shared::PushArrayToVector(output->GetHashArr(), HASH_SIZE, name_data_);

    name_ = Encryption::GenerateDHash(name_data_);
}