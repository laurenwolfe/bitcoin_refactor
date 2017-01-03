#include "Shared.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cstdlib>

namespace Shared {

    uint32_t BinToDecimal(const std::vector <uint8_t> &binary_data, const uint32_t size) {
        int decimal = 0;
        for (int i = size - 1; i >= 0; i--) {
            decimal = (decimal << 8) + binary_data[i];
            //printf("%02x ", binary_data[i]);
        }

        return decimal;
    }

    uint32_t BinToDecimal(uint8_t *binary_data, const uint32_t size) {
        int decimal = 0;
        for (int i = size - 1; i >= 0; i--) {
            decimal = (decimal << 8) + binary_data[i];
            //printf("%02x ", binary_data[i]);
        }

        return decimal;
    }

    std::string BinToString(const std::vector<uint8_t> &binary_data) {
        std::string str (binary_data.begin(), binary_data.end());
        return str;
    }

    std::string BinToString(uint8_t *binary_data, uint8_t size) {
        std::stringstream stream;

        for (int i = 0; i < size; i++) {
            stream << std::setfill('0') << std::setw(2) << std::hex << unsigned(binary_data[i]);
        }

        std::string hex_str (stream.str());

        return hex_str;
    }

    std::string BinToHex(const std::vector <uint8_t> &binary_data, uint32_t size) {
        std::stringstream stream;

        for (int i = 0; i < binary_data.size(); i++) {
            stream << std::setfill('0') << std::setw(2) << std::hex << unsigned(binary_data[i]);
        }

        std::string hex_str (stream.str());

        return hex_str;
    }

    std::string BinToBitString(const std::vector<uint8_t> &binary_data) {
        std::string bit_str;

        for(int i = 0; i < binary_data.size(); i++) {
            std::bitset<CHAR_BIT> bits(binary_data[i]);
            bit_str.append(bits.to_string());
            std::cout << "uint8_t: " << int(binary_data[i]) << " "
                      << "byte: " << bits.to_string() << std::endl;
        }

        return bit_str;
    }

     void StringToArray(const std::string &str, uint8_t *arr) {
         int j = 0;

         for(int i = 0; i < str.size(); i = i + 2) {
             std::string substr = str.substr(i, 2);

             arr[j] = (uint8_t)(std::stoi(substr, nullptr, 16));
             j++;
         }
    }
}