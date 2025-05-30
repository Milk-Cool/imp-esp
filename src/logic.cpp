#include "logic.h"
#include "util.h"
#include "transport.h"

#if TYPE == 1
static String phone_number;

static void challenge_respond(vector<uint8_t> rand_data, HandleCtx ctx) {
    EncData enc = encrypt_only(ctx.key, rand_data);
    vector<uint8_t> vec;
    write_buf(vec, enc.nonce, 12);
    write_buf(vec, enc.authtag, 16);
    write_buf(vec, enc.data.data(), enc.data.size());

    vector<uint8_t> packdata = construct_packet({
        .id = 0x0001,
        .data = vec
    });
    transmit(packdata);
}

void handle_packet(Packet packet, HandleCtx& ctx) {
    switch (packet.id) {
    case 0x0000:
        challenge_respond(packet.data, ctx);
        break;
    case 0x0001:
        ctx.authenticated = true;
        break;
    case 0x0080:
        phone_number = read_str0(packet.data, 0);
        Serial.println("Phone number: " + phone_number);
        break;
    case 0x0100:
        Serial.print("Message from ");
        Serial.print(read_str0(packet.data, 0));
        Serial.print(": ");
        Serial.println(read_str0(packet.data, 0));
        break;
    default:
        break;
    }
}

String get_my_number() {
    return phone_number;
}

void send_message(uint8_t* key, String num, String msg) {
    vector<uint8_t> packet_data;
    write_str0(packet_data, num);
    write_str0(packet_data, msg);
    Packet packet = {
        .id = 0x0100,
        .data = packet_data
    };

    vector<uint8_t> encrypted = encrypt_packet(key, packet);
    transmit(encrypted);
}
#endif