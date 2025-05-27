#pragma once
#include "protocol.h"

#if TYPE == 1
typedef struct {
    uint8_t* key;
    bool authenticated;
} HandleCtx;
void handle_packet(Packet packet, HandleCtx& ctx);
#endif