#pragma once
#include <vector>
#include <stdint.h>
using namespace std;

#if TYPE == 1
typedef struct {
    uint16_t id;
    vector<uint8_t> data;
} Packet;
typedef struct {
    uint8_t nonce[12];
    uint8_t authtag[16];
    vector<uint8_t> data;
} EncData;

int init_rng();

vector<uint8_t> construct_packet(Packet packet);
EncData encrypt_only(uint8_t key[32], vector<uint8_t> data);
vector<uint8_t> encrypt_packet(uint8_t key[32], Packet packet);
Packet deconstruct_packet(vector<uint8_t> data);
Packet deconstruct_packet(uint8_t* data, size_t len);
vector<uint8_t> decrypt_only(uint8_t key[32], EncData data);
Packet decrypt_packet(uint8_t key[32], vector<uint8_t> data);
Packet decrypt_packet(uint8_t key[32], uint8_t* data, size_t len);

vector<Packet> get_all_packets(vector<uint8_t>& data);
vector<Packet> get_all_enc_packets(uint8_t key[32], vector<uint8_t>& data);
#endif