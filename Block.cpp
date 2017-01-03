#include <iostream>
#include <math.h>
#include <time.h>

#include "BTCMiner.h"
#include "Block.h"
#include "Encryption.h"
#include "Shared.h"

using namespace std;

Block::Block(const vector<uint8_t> &data,
             map<string, uint32_t> &bitcoin_ledger,
             map<string, uint32_t> &txn_ledger,
             uint32_t &idx,
             uint8_t prev_header_hash[HASH_SIZE])
        : data_(data), bitcoin_ledger_(bitcoin_ledger), txn_ledger_(txn_ledger),
          idx_(idx), start_idx_(idx) {

    for(int i = 0; i < HASH_SIZE; i++) {
        prev_header_hash_[i] = prev_header_hash[i];
    }

    //string hash = Shared::BinToString(prev_header_hash_, HASH_SIZE);
    //cout << "hash: " << hash << endl;
}

uint32_t Block::ParseGenesisBlock(uint8_t *empty_header_hash) {
    cout << "Genesis header..." << endl;
    idx_ += Shared::Slice(data_, header_, idx_, HEADER_SIZE);

    HashHeader();

    cout << "block name: " << block_name_ << endl;

    //what is this?!
    //Encryption::GenerateDHash(header_, HEADER_SIZE, prev_header_hash_);

    idx_ += Shared::Slice(data_, txn_count_, idx_, INT_SIZE);

    GetTxns(Shared::BinToDecimal(txn_count_, INT_SIZE), processed_txns_);

    return idx_;
}

uint32_t Block::ParseData(uint32_t txn_count, uint32_t &processed_txns) {
    MakeCoinbaseTxn();
    //todo switch back!!
//    GetTxns(txn_count, processed_txns);
    GetTxns(5, processed_txns);

    return idx_;
}

void Block::MakeCoinbaseTxn() {
    cout << "Make coinbase txn..." << endl;

    //txns_.size() == 0 here, and this is the 0 index txn
    class Transaction *coinbase = new class Transaction(data_, txn_ledger_, txns_.size(), idx_);
    coinbase->MakeCoinbaseTxn();

    txns_.push_back(coinbase);
}

bool Block::GetTxns(uint32_t count, uint32_t &processed_txns) {
    cout << "get txns count: " << count << endl;
    if(count <= 0) {
        cerr << "Invalid txn count." << endl;
        return false;
    }

    //txn_hashes_.reserve(count);

    //Parse transactions
    for(uint32_t i = 0; i < count; i++) {
        if(i % 10 == 0) {
            cout << i  << " txns processed." << endl;
        }

        uint32_t txn_start_idx = idx_;

        class Transaction *txn =
                new class Transaction(data_, txn_ledger_, i, idx_);

        cout << "<<<<<<<<<< " << "Transaction # " << i << endl;
        bool result = txn->ParseTransaction();
        cout << ">>>>>>>>>>" << endl;


        cout << "start idx: " << txn_start_idx << ", end idx: " << idx_ << endl;
        cout << "txn name hash: " << txn->GetName() << endl;

        if(!result) {
            cout << "Txn input(s) invalid" << endl;
            delete txn;
        } else {
            txns_.push_back(txn);
        }

        processed_txns = i;
    }

    return true;
}

/*
 * todo
bool Block::CalculateMerkleRoot() {
    if(txn_hashes_.size() == 0)
        return false;

    uint32_t idx = txn_hashes_.size() - 1;
    vector<uint8_t> parent, tmp;

    while(1) {
        uint32_t parent_idx = (idx - 1) / 2;

        if(idx % 2 == 0) {
            ConcatVector(txn_hashes_[idx], txn_hashes_[idx]);
            txn_hashes_[parent_idx] = txn_hashes_[idx];
            idx--;
        } else {
            ConcatVector(txn_hashes_[idx - 1], txn_hashes_[idx]);
            txn_hashes_[parent_idx] = txn_hashes_[idx - 1];
            idx = idx - 2;
        }


        if(parent_idx == 0)
            merkle_root_ = Encryption::GenerateDHashVector(txn_hashes_[parent_idx]);
            break;
    }

    return true;
}
*/

/*
 * todo
bool Block::ConstructHeader() {
    CalculateMerkleRoot();

    vector<unsigned char> time_vector;
    //Insert version #
    header_.push_back(1);
    header_.push_back(0);
    header_.push_back(0);
    header_.push_back(0);
    ConcatVector(header_, prev_header_hash_);
    ConcatVector(header_, merkle_root_);

    GetUnixTime(time_vector);

    for(auto it = time_vector.begin(); it != time_vector.end(); ++it) {
        header_.push_back(*it);
    }

    header_.push_back(3);
    header_.push_back(0);

    FindNonce(header_);

    return true;
}
*/

/*
 * todo
void Block::GetUnixTime(vector<unsigned char > &time_vector) {
    time_t timer;
    time(&timer);
    unsigned char bytes[sizeof timer];
    std::copy(static_cast<const char*>(static_cast<const void*>(&timer)),
              static_cast<const char*>(static_cast<const void*>(&timer)) + sizeof timer,bytes);

    for(int i = 0; i < 4; i++) {
        time_vector[i] = bytes[i];
        //cout << i << ": " << (int)bytes[i] << " ";
    }
}
*/

/*
 * todo
void Block::ConcatVector(vector<uint8_t> left, vector<uint8_t> right) {
    left.insert(left.end(), right.begin(), right.end());

}
*/
//passing by value to avoid corrupting the other one?
/*
 * todo
bool Block::FindNonce(vector<uint8_t> header_cpy) {
    int ctr = 0;
    while(1) {
        if(ctr % 1000 == 0) {
            cout << "Tried " << ctr << " nonces..." << endl;
        }

        for (int i = 0; i < sizeof(uint8_t); i++) {
            header_.push_back(rand() % 255);
        }

        vector <uint8_t> header_hash = Encryption::GenerateDHashVector(header_);

        for (int j = 7; j > 4; j--) {
            if (header_hash[j] != 0) {
                for (int k = 0; k < 8; k++)
                    header_hash.pop_back();
                ctr++;
                continue;
            }
        }
    }
}
*/

/*
 * todo
bool Block::InsertTxn(Transaction txn) {
//Look for matching entry in txn_ledger
// Verify that output matches input
auto txn_it = txn_ledger_.find(input.key_hash);
if(txn_it != txn_ledger_.end()) {
    int output_txn_idx = txn_it->second;
    const OutputSpecifier &source = txns_[output_txn_idx].
            output_specifiers_[input.output_spec_idx];

    if(source.recipient_key_hash != input.key_hash) {
        return false;
    } else {
        input.num_bitcoins = source.btc_amount;
        input_sum_ += input.num_bitcoins;
    }
} else {
    return false;
}
    //todo: adjust bitcoin ledger and remove claimed outputs from txn_ledger
    return false;
}
*/

bool Block::HashHeader() {
    block_name_ = Encryption::GenerateDHash(header_, HEADER_SIZE, header_hash_);
    return true;
}
//todo: calculate block name