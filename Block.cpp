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
             unordered_map<string, Transaction *> &txns,
             unordered_map<string, set<uint32_t>> &unused_outputs,
             unordered_map<string, uint32_t> &bitcoin_ledger,
             uint32_t &idx,
             uint32_t total_txns,
             uint32_t &processed_txns,
             uint8_t *prev_header_hash)
        : data_(data), txns_(txns), unused_outputs_(unused_outputs),
          bitcoin_ledger_(bitcoin_ledger), idx_(idx), start_idx_(idx),
          total_txns_(total_txns), processed_txns_(processed_txns) {

    //write prev header name
    for(int i = 0; i < HASH_SIZE; i++) {
        prev_header_hash_[i] = prev_header_hash[i];
    }

    block_txn_keys_.reserve(100);
}

uint32_t Block::ParseGenesisBlock() {
    uint8_t output_txn_count_[INT_SIZE] = {0};

    cout << "Genesis header..." << endl;
    idx_ += Shared::Slice(data_, header_, idx_, HEADER_SIZE);
    idx_ += Shared::Slice(data_, output_txn_count_, idx_, INT_SIZE);

    //Block header hash (block name)
    block_name_ = Encryption::GenerateDHash(header_, HEADER_SIZE, header_hash_);

    GetTxns(true);

    return idx_;
}

uint32_t Block::ParseData() {
    GetTxns(false);
    return idx_;
}

bool Block::GetTxns(bool is_genesis) {
    bool valid_txn = true;

    //Parse transactions
    for(uint32_t i = processed_txns_; i < total_txns_; i++) {
        //break if max block size reached
        if(idx_ - start_idx_ > MAX_BLOCK_SIZE) {
            break;
        }

        if(i % 10 == 0)
            cout << i  << " txns processed." << endl;

        //temporarily store sub-transaction amounts to add to ledger
        //once input specifiers are verified
        vector<pair<string, int32_t>> bitcoin_adjustments;
        int32_t sum = 0;
        set<uint32_t> unused_output_idxs;

        class Transaction *txn = new class Transaction(data_, idx_);

        cout << "<<<<<<<<<< " << "Transaction # " << i << endl;
        txn->ParseTransaction();
        cout << ">>>>>>>>>>" << endl;

        if(txn->GetName() == "") {
            cout << "Txn input(s) invalid" << endl;
            valid_txn = false;
            break;
        }

        if(!is_genesis) {
            //Verify that transaction inputs are valid outputs
            for (int i = 0; i < txn->GetInputSpecs().size(); i++) {
                InputSpecifier input = *(txn->GetInputSpecs()[i]);

                string source_txn_hash = input.GetPrevTxnName();
                uint16_t out_idx = input.GetOutputIdx();

                auto source_txn_it = txns_.find(source_txn_hash);
                if(source_txn_it == txns_.end()) {
                    cout << "Transaction not found. " << endl;
                    valid_txn = false;
                    break;
                }

                Transaction *source_txn = source_txn_it->second;

                auto source_txn_unused_idxs_it = unused_outputs_.find(source_txn_hash);
                if(source_txn_unused_idxs_it == unused_outputs_.end()) {
                    cout << "Transaction not found in unused outputs. " << endl;
                    valid_txn = false;
                    break;
                }
                set<uint32_t> unused_idxs = source_txn_unused_idxs_it->second;

                auto unused_idx_it = unused_idxs.find(out_idx);
                if(unused_idx_it == unused_idxs.end()) {
                    cout << "Transaction already used. " << endl;
                    valid_txn = false;
                    break;
                }

                OutputSpecifier *output = source_txn->GetOutputSpecs()[out_idx];

                string key_hash = input.GetKeyHash();

                if(key_hash != output->GetKeyHash()) {
                    cout << "Input and output hashes don't match. " << endl;
                    valid_txn = false;
                    break;
                }

                //Verify that the input spec signature is valid for public key
                if (!Encryption::VerifySignature(input.GetPublicKeyVector(),
                                                 input.GetTxnSignature(),
                                                 txn->GetSignatureDataVector())) {
                    cout << "Signature not verified. " << endl;
                    valid_txn = false;
                    break;
                }

                // Convert to negative btc value and add to sub-transactions
                // (for btc ledger manipulation)
                int32_t btc = (int32_t)(output->GetBtcAmount());
                int32_t neg_btc = btc * -1;

                cout << "input subtracts " << neg_btc << " bitcoins. " << endl;

                pair<string, int32_t> sub_transaction = make_pair(key_hash, neg_btc);

                bitcoin_adjustments.push_back(sub_transaction);
            }

            //fetch output sub-transactions and add to bitcoin adjustment list
            for (int j = 0; j < txn->GetOutputSpecs().size(); j++) {
                OutputSpecifier *curr_output = txn->GetOutputSpecs()[j];

                pair <string, int32_t> sub_trans = make_pair(curr_output->GetKeyHash(),
                                  (int32_t)(curr_output->GetBtcAmount()));
                bitcoin_adjustments.push_back(sub_trans);

                unused_output_idxs.insert(j);
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
            if(is_genesis && sum < 0) {
                cout << "Awarded miner " << sum << " btcs." << endl;
                //subtracting a negative number, so actually adding
                bitcoin_ledger_[my_hash_] -= sum;
            }

            //adjust the bitcoin ledger sums
            for(int k = 0; k < bitcoin_adjustments.size(); k++) {
                int32_t balance;

                auto ledger_entry_it = bitcoin_ledger_.find(bitcoin_adjustments[k].first);
                if(ledger_entry_it != bitcoin_ledger_.end()) {
                    balance = ledger_entry_it->second;
                } else {
                    balance = 0;
                }
                bitcoin_ledger_[bitcoin_adjustments[k].first] =
                        balance + bitcoin_adjustments[k].second;
            }

            //Delete output index from unused list
            for(int i = 0; i < txn->GetInputSpecs().size(); i++) {
                InputSpecifier input = *(txn->GetInputSpecs()[i]);
                auto unused_it = unused_outputs_.find(input.GetPrevTxnName());
                if(unused_it != unused_outputs_.end()) {
                    unused_it->second.erase(input.GetOutputIdx());
                }
            }

            //insert transaction into maps
            txns_[txn->GetName()] = txn;
            unused_outputs_[txn->GetName()] = unused_output_idxs;
            block_txn_keys_.push_back(txn->GetName());
        }

        processed_txns_ = i;
    }
    return true;
}

Transaction *Block::MakeCoinbaseTxn() {
    //txns_.size() == 0 here, and this is the 0 index txn
    class Transaction *coinbase = new class Transaction(data_, idx_);
    coinbase->MakeCoinbaseTxn();

    return coinbase;
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