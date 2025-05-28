#include "transport.h"
#include <WiFi.h>
#include <AsyncTCP.h>
#include "util.h"

// TODO:
// errors on init
// timeouts

#if TYPE == 0 && TRANSPORT == 0
static AsyncServer server(PORT_RELAY);

typedef struct {
    AsyncClient* client;
    vector<uint8_t>* buf;
    bool* connected;
} CtxWithConnectInfo;

typedef struct {
    AsyncClient* upstream;
    vector<uint8_t>* buf;
    bool* connected;

    CtxWithConnectInfo* upstream_ctx;
    CtxWithConnectInfo* client_ctx;
} FullCtx;

void init_relay() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(IMP_SSID, IMP_PASS);

    WiFi.begin(IMP_UPSTREAM_SSID, IMP_UPSTREAM_PASS);
    while(WiFi.status() != WL_CONNECTED) delay(10);

    server.onClient([](void* arg, AsyncClient* client) {
        (void)arg;

        AsyncClient* upstream = new AsyncClient();

        vector<uint8_t>* buf_before_connect = new vector<uint8_t>();
        bool* upstream_connected = new bool(false);

        CtxWithConnectInfo* ctx_upstream = new CtxWithConnectInfo({
            .client = upstream,
            .buf = buf_before_connect,
            .connected = upstream_connected
        });

        CtxWithConnectInfo* ctx_client = new CtxWithConnectInfo({
            .client = client,
            .buf = buf_before_connect,
            .connected = upstream_connected
        });

        FullCtx* full_ctx = (FullCtx*)malloc(sizeof(FullCtx));
        full_ctx->buf = buf_before_connect;
        full_ctx->connected = upstream_connected;
        full_ctx->upstream = upstream;
        full_ctx->upstream_ctx = ctx_upstream;
        full_ctx->client_ctx = ctx_client;

        upstream->connect(ADDR_SERVER, PORT_SERVER);
        upstream->onConnect([](void* ctx_void, AsyncClient* upstream) {
            CtxWithConnectInfo* ctx = (CtxWithConnectInfo*)ctx_void;
            *ctx->connected = true;
            if(ctx->buf->empty()) return;
            upstream->write((const char*)ctx->buf->data(), ctx->buf->size());
            ctx->buf->clear();
        }, ctx_client);
        upstream->onData([](void* client_void, AsyncClient* upstream, void* data, size_t len) {
            AsyncClient* client = (AsyncClient*)client_void;
            client->write((const char*)data, len);
        }, client);
        // TODO: upstream->onDisconnect just in case
        client->onData([](void* ctx_void, AsyncClient* client, void* data, size_t len) {
            CtxWithConnectInfo* ctx = (CtxWithConnectInfo*)ctx_void;

            for(size_t i = 0; i < len; i++)
                Serial.printf("%02x ", ((uint8_t*)data)[i]);
            Serial.println();

            if(*ctx->connected)
                ctx->client->write((const char*)data, len);
            else
                write_buf(*ctx->buf, (uint8_t*)data, len);
        }, ctx_upstream);
        client->onDisconnect([](void* full_ctx_void, AsyncClient* client) {
            FullCtx* full_ctx = (FullCtx*)full_ctx_void;

            full_ctx->upstream->close();
            full_ctx->upstream->free();
            delete full_ctx->upstream;

            delete full_ctx->upstream_ctx;
            delete full_ctx->client_ctx;

            delete full_ctx->buf;
            delete full_ctx->connected;

            free(full_ctx_void);
        }, full_ctx);
    }, NULL);

    server.begin();
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