#pragma once
#include "protocol.h"
#include <Arduino.h>

#if TYPE == 1
typedef struct {
    uint8_t* key;
    bool authenticated;
    String phone_number;
} HandleCtx;
void handle_packet(Packet packet, HandleCtx& ctx);
#endif