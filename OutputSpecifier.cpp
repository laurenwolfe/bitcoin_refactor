#include "OutputSpecifier.h"

using namespace std;

OutputSpecifier::OutputSpecifier(std::vector <uint8_t> data, uint32_t &idx, bool is_coinbase) : data_(data), idx_(idx) {
    if(is_coinbase) {
        std::string hashed_key ("1f5a0200bc94ae4264642855786d9c2bb436b9e129ef95e6416136c03f339581");

        btc_amount_[0] = 0xa;
        btc_amount_[1] = 0x0;
        btc_amount_[2] = 0x0;
        btc_amount_[3] = 0x0;

        Shared::StringToArray(hashed_key, recipient_key_hash_);
    } else {
        idx_ += Shared::Slice(data_, btc_amount_, idx_, INT_SIZE);
        idx_ += Shared::Slice(data_, recipient_key_hash_, idx_, HASH_SIZE);
    }
}

void OutputSpecifier::Print() {
    cout << "btc amount: " << Shared::BinToDecimal(btc_amount_, INT_SIZE) << endl;
    cout << "recipient key hash: " << Shared::BinToString(recipient_key_hash_, HASH_SIZE) << endl;
}