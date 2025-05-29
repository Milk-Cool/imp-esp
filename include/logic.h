#pragma once
#include "protocol.h"
#include <Arduino.h>

#if TYPE == 1
typedef struct {
    uint8_t* key;
    bool authenticated;
} HandleCtx;
void handle_packet(Packet packet, HandleCtx& ctx);

String get_my_number();

void send_message(uint8_t* key, String num, String msg);
#endif