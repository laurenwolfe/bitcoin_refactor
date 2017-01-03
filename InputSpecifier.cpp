#include "InputSpecifier.h"
#include "Shared.h"
#include "Encryption.h"

using namespace std;

InputSpecifier::InputSpecifier(std::vector<uint8_t> data, uint32_t &idx) : data_(data), idx_(idx) {
    idx_ += Shared::Slice(data_, prev_txn_name_, idx_, HASH_SIZE);
    idx_ += Shared::Slice(data_, output_spec_idx_, idx_, SHORT_SIZE);
    idx_ += Shared::Slice(data_, txn_signature_, idx_, SIG_SIZE);
    idx_ += Shared::Slice(data_, key_len_, idx_, SHORT_SIZE);

    WritePublicKey();
}

void InputSpecifier::WritePublicKey() {
    uint32_t key_len_int = Shared::BinToDecimal(key_len_, SHORT_SIZE);

    public_key_.reserve(key_len_int);

    for (uint32_t i = idx_; i < (idx_ + key_len_int); i++) {
        public_key_.push_back(data_[i]);
    }

    idx_ += key_len_int;

    key_hash_ = Encryption::GenerateDHash(public_key_);
}

void InputSpecifier::Print() {
    cout << "Prev txn name: " << Shared::BinToString(prev_txn_name_, HASH_SIZE) << endl;
    cout << "Output spec idx: " << Shared::BinToDecimal(output_spec_idx_, SHORT_SIZE) << endl;
    cout << "Signature: " << Shared::BinToString(txn_signature_, SIG_SIZE) << endl;
    //cout << "Key: " << Shared::BinToString(public_key_) << endl;
    cout << "Key Hash: " << key_hash_ << endl;
}