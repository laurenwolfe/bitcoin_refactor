#ifndef BITCOIN_SHARED_HPP
#define BITCOIN_SHARED_HPP

#include <cstdint>
#include <vector>
#include <string>
#include <stdio.h>
#include <bitset>
#include <type_traits>
#include <iostream>

namespace Shared {
    uint32_t BinToDecimal(const std::vector<uint8_t> &binary_data,
                          const uint32_t size);
    uint32_t BinToDecimal(uint8_t *binary_data, const uint32_t size);

    std::string BinToHex(const std::vector<uint8_t> &binary_data, uint32_t size);
    std::string BinToBitString(const std::vector<uint8_t> &binary_data);
    std::string BinToString(const std::vector<uint8_t> &binary_data);
    std::string BinToString(uint8_t *binary_data, uint8_t size);
    void StringToArray(const std::string &str, uint8_t *arr);

    //writes to arrays containing generic types
    template<class T>
    uint32_t Slice(const std::vector <uint8_t> &data, T &slice, const uint32_t idx,
                   const uint32_t size) {
        uint32_t j = 0;
        for (uint32_t i = idx; i < idx + size; i++) {
            slice[j] = data[i];
            j++;
        }

        return size;
    }
}
#endif // BITCOIN_SHARED_HPP