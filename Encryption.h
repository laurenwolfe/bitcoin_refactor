#ifndef BTC_MINER_ENCRYPTION_H
#define BTC_MINER_ENCRYPTION_H

#include <vector>
#include <cstdint>
#include "Botan/botan_all.h"

namespace Encryption {
    std::string GenerateDHash(std::vector<uint8_t> &byte_vector);
    std::string GenerateDHash(uint8_t *byte_arr, uint32_t size, uint8_t *hash_arr);
    std::string GenerateDHash(std::string str, uint8_t *hash_arr);

    std::vector<uint8_t> GenerateDHashVector(std::vector<uint8_t> &byte_vector);

    bool VerifySignature(const std::vector<uint8_t> &public_key,
                         uint8_t *signature,
                         const std::vector<uint8_t> &data);

    Botan::Public_Key *ImportPublicKey(std::vector<uint8_t> key_bytes);
}
#endif //BTC_MINER_ENCRYPTION_H