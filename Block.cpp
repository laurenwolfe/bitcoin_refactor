#include <iostream>
#include <math.h>
#include <time.h>
#include <utility>

#include "BTCMiner.h"
#include "Block.h"
#include "Encryption.h"
#include "Shared.h"
#include "InputSpecifier.h"
#include "OutputSpecifier.h"

using namespace std;

//todo: convert header_, merkle_root_, header_hash_ from vector to array
//todo: convert ConcatVector to insert() statement, where it might still be used
//todo: output block binary from block_txn_keys values, include coinbase txn

Block::Block(const vector<uint8_t> &data,
             unordered_map<string, Transaction> &txns,
             unordered_map<string, set<uint32_t>> &unused_outputs,
             unordered_map<string, uint32_t> &bitcoin_ledger,
             uint32_t &idx,
             uint32_t total_txns,
             uint32_t &processed_txns,
             uint8_t *prev_header_hash)
        : data_(data), txns_(txns), unused_outputs_(txn_ledger),
          bitcoin_ledger_(bitcoin_ledger), idx_(idx), start_idx_(idx),
          total_txns_(total_txns), processed_txns_(processed_txns) {

    //write prev header name
    for(int i = 0; i < HASH_SIZE; i++) {
        prev_header_hash_[i] = prev_header_hash[i];
    }

    block_txn_keys_.reserve(100);
}

uint32_t Block::ParseGenesisBlock() {
    uint8_t output_txn_count_ = {0};

    cout << "Genesis header..." << endl;
    idx_ += Shared::Slice(data_, header_, idx_, HEADER_SIZE);
    idx_ += Shared::Slice(data_, output_txn_count_, idx_, INT_SIZE);

    //Block header hash (block name)
    block_name_ = Encryption::GenerateDHash(header_, HEADER_SIZE, header_hash_);

    GetTxns(false);

    return idx_;
}

uint32_t Block::ParseData() {
    GetTxns(true);
    return idx_;
}

bool Block::GetTxns(bool has_inputs) {
    bool valid_txn = true;

    //cout << "get txns total_txns_: " << total_txns_ << endl;
    if(total_txns_ <= 0) {
        cerr << "Invalid txn count." << endl;
        return false;
    }

    //Parse transactions
    for(uint32_t i = processed_txns_; i < total_txns_; i++) {
        //break if max block size reached
        if(idx_ - start_idx_ > MAX_BLOCK_SIZE) {
            break;
        }

        //temporarily store sub-transaction amounts
        vector<pair<string, int32_t>> bitcoin_adjustments;
        int32_t sum = 0;
        string input_hash;

        if(i % 10 == 0)
            cout << i  << " txns processed." << endl;

        uint32_t txn_start_idx = idx_;

        class Transaction *txn = new class Transaction(data_, txn_ledger_, i, idx_);

        cout << "<<<<<<<<<< " << "Transaction # " << i << endl;
        txn->ParseTransaction();
        cout << ">>>>>>>>>>" << endl;

        if(txn->GetName() == "") {
            cout << "Txn input(s) invalid" << endl;
            valid_txn = false;
            break;
        }

        if(has_inputs) {
            //Verify that transaction inputs are valid outputs
            for (int i = 0; i < txn->GetInputSpecs().size(); i++) {
                uint32_t txn_idx;
                string txn_name = txn->GetInputSpecs()[i]->GetPrevTxnName();
                uint16_t out_idx = txn->GetInputSpecs()[i]->GetOutputIdx();
                string key_hash = txn->GetInputSpecs()[i]->GetKeyHash();

                //verify that input transaction is a valid output transaction;
                //it should exist in the ledger and should not have been flagged as used already
                auto txn_ledger_res = txn_ledger_.find(txn_name);
                if (txn_ledger_res == txn_ledger_.end()) {
                    cout << "Transaction not found in the ledger. " << endl;
                    valid_txn = false;
                    break;
                }
                if (txn_ledger_res->second[out_idx]) {
                    cout << "Transaction already used. " << endl;
                    valid_txn = false;
                    break;
                }

                //fetch transaction index from map of transaction names -> index in transaction vector
                auto txn_idx_res = txn_idxs_.find(txn_name);
                if (txn_idx_res == txn_idxs_.end()) {
                    cout << "Transaction not found in the transaction index map. " << endl;
                    valid_txn = false;
                    break;
                }

                txn_idx = txn_idx_res->second;

                Transaction *out_txn = txns_[txn_idx];

                //If input and output hashes don't match, not a valid transaction
                if (key_hash != out_txn->GetOutputSpecs()[out_idx]->GetKeyHash()) {
                    cout << "Input and output hashes don't match. " << endl;
                    valid_txn = false;
                    break;
                }

                //Verify that the input spec signature is valid for public key
                if (!Encryption::VerifySignature(txn->GetInputSpecs()[i]->GetPublicKeyVector(),
                                                 txn->GetInputSpecs()[i]->GetTxnSignature(),
                                                 txn->GetSignatureDataVector())) {
                    cout << "Signature not verified. " << endl;
                    valid_txn = false;
                    break;
                }

                // Convert to negative btc value and add to sub-transactions (for btc ledger manipulation)
                int32_t btc = (int32_t)(txn->GetOutputSpecs()[out_idx]->GetBtcAmount());
                int32_t neg_btc = btc * -1;

                cout << "negative btc value for input subtransaction: " << neg_btc << endl;

                pair <string, int32_t> sub_transaction = make_pair(key_hash, neg_btc);

                bitcoin_adjustments.push_back(sub_transaction);
            }

            //fetch output sub-transactions and get sum
            for (int j = 0; j < txn->GetOutputSpecs().size(); j++) {
                pair <string, int32_t> sub_trans =
                        make_pair(txn->GetOutputSpecs()[j]->GetKeyHash(),
                                  (int32_t)(txn->GetOutputSpecs()[j]->GetBtcAmount()));
                bitcoin_adjustments.push_back(sub_trans);
            }

            for (int k = 0; k < bitcoin_adjustments.size(); k++) {
                sum += bitcoin_adjustments[k].second;
            }

            //Verify that output sum doesn't exceed input sum
            if (sum > 0) {
                cout << "Output bitcoin amount exceeds input bitcoin amount. " << endl;

                valid_txn = false;
                break;
            }
        }

        if(valid_txn) {
            //If transaction has leftover btcs, award to miner
            if(has_inputs && sum < 0) {
                cout << "Awarded myself " << sum << " btcs." << endl;
                //subtracting a negative number, so actually adding
                bitcoin_ledger_[my_hash_] -= sum;
            }

            //adjust the bitcoin ledger sums
            for(int k = 0; k < bitcoin_adjustments.size(); k++) {
                bitcoin_ledger_[bitcoin_adjustments[k].first] += bitcoin_adjustments[k].second;
            }

            //Flag transaction output as used
            for(int i = 0; i < txn->GetInputSpecs().size(); i++) {
                auto it = txn_ledger_.find(txn->GetInputSpecs()[i]->GetPrevTxnName());
                it->second[txn->GetInputSpecs()[i]->GetOutputIdx()] = true;
            }

            // transaction name -> txns_[] index
            txn_idxs_[txn->GetName()] = txns_.size();
            txns_.push_back(txn);
        }

        processed_txns_ = i;
    }

    cout << "Txn count: " << txns_.size() << endl;

    return true;
}

void Block::MakeCoinbaseTxn() {
    //txns_.size() == 0 here, and this is the 0 index txn
    class Transaction *coinbase = new class Transaction(data_, txn_ledger_, txns_.size(), idx_);
    coinbase->MakeCoinbaseTxn();

    txn_idxs_[coinbase->GetName()] = txns_.size();
    txns_.push_back(coinbase);
}

/*
 * todo
bool Block::CalculateMerkleRoot() {
 //todo: use txns_[i].GetNameHashArray() instead of txn_hashes_
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
bool Block::FindNonce(const vector<uint8_t> &header_cpy) {
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