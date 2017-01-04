#include "BTCMiner.h"
#include "Encryption.h"
#include "Shared.h"

#include <sstream>

namespace Encryption {
    std::string GenerateDHash(std::vector<uint8_t> &byte_vector) {
        if(byte_vector.size() == 0) {
            std::cerr << "Can't generate Hash, vector is empty." << std::endl;
        }
        std::stringstream stream;
        std::unique_ptr<Botan::HashFunction> first_hash(Botan::HashFunction::create("SHA-256"));

        Botan::secure_vector<Botan::byte> hash1 = first_hash->process(byte_vector);

        std::unique_ptr<Botan::HashFunction> dHash(Botan::HashFunction::create("SHA-256"));
        Botan::secure_vector<Botan::byte> hash2 = first_hash->process(hash1);

        stream << Botan::hex_encode(hash2, false);
        return stream.str();

    }

    std::string GenerateDHash(uint8_t *byte_arr, uint32_t size, uint8_t *hash_arr) {
        std::stringstream stream;
        std::unique_ptr<Botan::HashFunction> first_hash(Botan::HashFunction::create("SHA-256"));

        Botan::secure_vector<Botan::byte> hash1 = first_hash->process(byte_arr, size);

        std::unique_ptr<Botan::HashFunction> dHash(Botan::HashFunction::create("SHA-256"));
        Botan::secure_vector<Botan::byte> hash2 = first_hash->process(hash1);

        stream << Botan::hex_encode(hash2, false);

        Shared::StringToArray(stream.str(), hash_arr);

        return stream.str();
    }

    std::string GenerateDHash(std::string str, uint8_t *hash_arr) {
        std::stringstream stream;
        std::vector<uint8_t> int_vec, final_vec;

        std::unique_ptr<Botan::HashFunction> first_hash(Botan::HashFunction::create("SHA-256"));
        first_hash->update(str);
        first_hash->final(int_vec);

        std::unique_ptr<Botan::HashFunction> dHash(Botan::HashFunction::create("SHA-256"));
        dHash->update(int_vec);
        dHash->final(final_vec);

        stream << Botan::hex_encode(final_vec, false);

        Shared::StringToArray(stream.str(), hash_arr);

        return stream.str();
    }

    std::vector<uint8_t> GenerateDHashVector(std::vector<uint8_t> &byte_vector) {
        std::vector<uint8_t> int_vec, final_vec;

        std::unique_ptr<Botan::HashFunction> first_hash(Botan::HashFunction::create("SHA-256"));
        first_hash->update(byte_vector);
        first_hash->final(int_vec);

        std::unique_ptr<Botan::HashFunction> dHash(Botan::HashFunction::create("SHA-256"));
        dHash->update(int_vec);
        dHash->final(final_vec);

        return final_vec;
    }

    bool VerifySignature(const std::vector<uint8_t> &public_key,
                         uint8_t *signature,
                         const std::vector<uint8_t> &data) {
        Botan::Public_Key *pub_key = Botan::X509::load_key(public_key);
        Botan::PK_Verifier verifier(*pub_key, "EMSA4(SHA-256)");
        verifier.update(data);
        return verifier.check_signature(signature, SIG_SIZE);
    }

    Botan::Public_Key *ImportPublicKey(std::vector<uint8_t> key_bytes) {
        std::string key(key_bytes.begin(), key_bytes.end());
        return Botan::X509::load_key(key);
    }
}