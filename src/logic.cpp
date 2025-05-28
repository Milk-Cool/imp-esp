#include "logic.h"
#include "util.h"
#include "transport.h"

#if TYPE == 1
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
    default:
        break;
    }
}
#endif