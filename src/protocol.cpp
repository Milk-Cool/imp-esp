#include "protocol.h"
#include "util.h"
#include <wolfssl.h>
#include <wolfssl/wolfcrypt/chacha20_poly1305.h>

#if TYPE == 1
static WC_RNG rng;
int init_rng() {
    return wc_InitRng(&rng);
}

vector<uint8_t> construct_packet(Packet packet) {
    vector<uint8_t> data;
    write_u16be(data, packet.id);
    write_u32be(data, packet.data.size());
    write_buf(data, packet.data.data(), packet.data.size());
    return data;
}
EncData encrypt_only(uint8_t key[32], vector<uint8_t> data) {
    EncData enc;
    enc.data.resize(data.size());
    wc_RNG_GenerateBlock(&rng, enc.nonce, 12);
    wc_ChaCha20Poly1305_Encrypt(key, enc.nonce, NULL, 0, data.data(), data.size(), enc.data.data(), enc.authtag);
    return enc;
}
vector<uint8_t> encrypt_packet(uint8_t key[32], Packet packet) {
    vector<uint8_t> constructed = construct_packet(packet);

    EncData enc = encrypt_only(key, constructed);
    vector<uint8_t> data;

    write_buf(data, enc.nonce, 12);
    write_buf(data, enc.authtag, 16);
    write_u32be(data, enc.data.size());
    write_buf(data, enc.data.data(), enc.data.size());
    return data;
}
Packet deconstruct_packet(vector<uint8_t> data) {
    return deconstruct_packet(data.data(), data.size());
}
Packet deconstruct_packet(uint8_t* data, size_t len) {
    vector<uint8_t> vec;
    write_buf(vec, data + 6, aread_u32be(data, 2));
    return {
        .id = aread_u16be(data, 0),
        .data = vec
    };
}
vector<uint8_t> decrypt_only(uint8_t key[32], EncData data) {
    vector<uint8_t> dec;
    dec.resize(data.data.size());
    wc_ChaCha20Poly1305_Decrypt(key, data.nonce, NULL, 0, data.data.data(), data.data.size(), data.authtag, dec.data());
    return dec;
}
Packet decrypt_packet(uint8_t key[32], vector<uint8_t> data) {
    return decrypt_packet(key, data.data(), data.size());
}
static Packet packerr = { .id = 0xfff7 };
Packet decrypt_packet(uint8_t key[32], uint8_t* data, size_t len) {
    EncData enc;
    enc.data.resize(len - 32);
    memcpy(enc.authtag, data + 12, 16);
    memcpy(enc.nonce, data, 12);
    memcpy(enc.data.data(), data, len - 32);
    vector<uint8_t> dec = decrypt_only(key, enc);
    return deconstruct_packet(dec);
}

vector<Packet> get_all_packets(vector<uint8_t>& data) {
    vector<Packet> vec;
    while(true) {
        if(data.size() < 6) break;
        uint32_t len = read_u32be(data, 2);
        if(data.size() < 6 + len) break;

        vector<uint8_t> subdata(data.begin(), data.begin() + 6 + len);
        vec.push_back(deconstruct_packet(subdata));
        data.erase(data.begin(), data.begin() + 6 + len);
    }
    return vec;
}
vector<Packet> get_all_enc_packets(uint8_t key[32], vector<uint8_t>& data) {
    vector<Packet> vec;
    while(true) {
        if(data.size() < 32) break;
        uint32_t len = read_u32be(data, 28);
        if(data.size() < 32 + len) break;

        vector<uint8_t> subdata(data.begin(), data.begin() + 32 + len);
        vec.push_back(decrypt_packet(key, subdata));
        data.erase(data.begin(), data.begin() + 32 + len);
    }
    return vec;
}
#endif