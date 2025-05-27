#pragma once
#include <vector>
#include <stdint.h>
using namespace std;

#if TYPE == 0
void init_relay();
#elif TYPE == 1
void init_client();
void transmit(vector<uint8_t>& data);
typedef void (*RecvListener)(uint8_t* data, size_t len);
typedef void (*ConnListener)();
void set_recv_listener(RecvListener listener);
void set_conn_listener(ConnListener listener);
#endif