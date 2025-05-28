#include <Arduino.h>
#if TYPE == 1
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#endif

#include "auth.h"
#include "transport.h"
#include "protocol.h"
#include "util.h"
#include "logic.h"

#define FAIL(x) { Serial.println(F("Failed to initialize " x)); while(true) delay(100); }
#define INIT(x) { Serial.println("Initialized " x); }

uint8_t id[16];
uint8_t key[32];
bool authenticated = false;

#if TYPE == 1
#define CS 5
#endif

vector<uint8_t> vec;
void setup() {
    Serial.begin(115200);

    #if TYPE == 1
    pinMode(CS, OUTPUT);

    if(!SD.begin(CS)) FAIL("SD card FAIL");
    INIT("SD card");
    uint8_t card_type = SD.cardType();
    if(card_type == CARD_NONE) FAIL("SD card (no card attached)");
    INIT("SD card check");

    if(!get_id(SD, id)) FAIL("ID");
    if(!get_key(SD, key)) FAIL("key");
    INIT("creds");
    #endif

    #if TYPE == 0
    init_relay();
    INIT("relay");
    #elif TYPE == 1
    if(init_rng() != 0) FAIL("rng");
    INIT("rng");

    set_recv_listener([](uint8_t* data, size_t len) {
        write_buf(vec, data, len);
        vector<Packet> packets;
        if(authenticated)
            packets = get_all_enc_packets(key, vec);
        else
            packets = get_all_packets(vec);

        HandleCtx ctx = {
            .key = key,
            .authenticated = authenticated
        };
        for(Packet& packet : packets)
            handle_packet(packet, ctx);
        
        // Applying changes
        authenticated = ctx.authenticated;
    });
    set_conn_listener([]() {
        vector<uint8_t> id_vec;
        write_buf(id_vec, id, 16);
        vector<uint8_t> packdata = construct_packet({
            .id = 0x0000,
            .data = id_vec
        });
        transmit(packdata);
    });

    init_client();
    INIT("client");
    #endif
}

void loop() {
    #if TYPE == 0
    relay_loop();
    #elif TYPE == 1
    // for(int i = 0; i < sizeof id; i++)
    //     Serial.printf("%02x ", id[i]);
    // Serial.println();
    // delay(100);
    recv_loop();
    delay(1);
    #endif
}