#include "transport.h"
#include <WiFi.h>
#include <AsyncTCP.h>

// TODO:
// errors on init
// timeouts

#if TYPE == 0 && TRANSPORT == 0
static AsyncServer server(PORT_RELAY);

void init_relay() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(IMP_SSID, IMP_PASS);

    WiFi.begin(IMP_UPSTREAM_SSID, IMP_UPSTREAM_PASS);
    while(WiFi.status() != WL_CONNECTED) delay(10);

    server.onClient([](void* arg, AsyncClient* client) {
        (void)arg;

        AsyncClient* upstream = new AsyncClient();

        upstream->connect(ADDR_SERVER, PORT_SERVER);
        upstream->onData([](void* client_void, AsyncClient* upstream, void* data, size_t len) {
            AsyncClient* client = (AsyncClient*)client_void;
            client->write((const char*)data, len);
        }, client);
        // TODO: upstream->onDisconnect just in case
        client->onData([](void* upstream_void, AsyncClient* client, void* data, size_t len) {
            AsyncClient* upstream = (AsyncClient*)upstream_void;
            upstream->write((const char*)data, len);
        }, upstream);
        client->onDisconnect([](void* upstream_void, AsyncClient* client) {
            AsyncClient* upstream = (AsyncClient*)upstream_void;
            upstream->close();
            upstream->free();
            delete upstream;
        }, upstream);
    }, NULL);
}
void relay_loop() {} // ESPAsyncTCP is ASYNC, so we don't need to fetch data in loop()
#elif TYPE == 1 && TRANSPORT == 0
static AsyncClient client;
typedef struct {
    ConnListener conn_listener;
    RecvListener recv_listener;
} Listeners;
static Listeners listeners = {
    .conn_listener = NULL,
    .recv_listener = NULL
};
void init_client() {
    WiFi.mode(WIFI_STA);

    WiFi.begin(IMP_SSID, IMP_PASS);
    while(WiFi.status() != WL_CONNECTED) delay(10);

    client.onConnect([](void* ctx, AsyncClient* client) {
        (void)client;
        if(((Listeners*)ctx)->conn_listener == NULL) return;
        ((Listeners*)ctx)->conn_listener();

        client->onData([](void* ctx, AsyncClient* client, void* data, size_t len) {
            (void)client;
            if(((Listeners*)ctx)->recv_listener == NULL) return;
            ((Listeners*)ctx)->recv_listener((uint8_t*)data, len);
        }, ctx);
    }, &listeners);
    client.connect(ADDR_RELAY, PORT_RELAY);
}
void transmit(vector<uint8_t>& data) {
    client.write((const char*)data.data(), data.size());
}
void set_recv_listener(RecvListener listener) {
    listeners.recv_listener = listener;
}
void set_conn_listener(ConnListener listener) {
    listeners.conn_listener = listener;
}
void recv_loop() {} // ESPAsyncTCP is ASYNC, so we don't need to fetch data in loop()
#endif