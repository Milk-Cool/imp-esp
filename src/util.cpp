#include "util.h"

void write_u16be(vector<uint8_t>& vec, uint16_t n) {
    vec.push_back((n >> 8) & 0xff);
    vec.push_back((n >> 0) & 0xff);
}
void write_u32be(vector<uint8_t>& vec, uint32_t n) {
    vec.push_back((n >> 24) & 0xff);
    vec.push_back((n >> 16) & 0xff);
    vec.push_back((n >>  8) & 0xff);
    vec.push_back((n >>  0) & 0xff);
}
void write_buf(vector<uint8_t>& vec, uint8_t* buf, size_t len) {
    for(size_t i = 0; i < len; i++)
        vec.push_back(buf[i]);  
}
uint16_t read_u16be(vector<uint8_t>& vec, size_t pos) {
    return aread_u16be(vec.data(), pos);
}
uint32_t read_u32be(vector<uint8_t>& vec, size_t pos) {
    return aread_u32be(vec.data(), pos);
}
uint16_t aread_u16be(uint8_t* arr, size_t pos) {
    return ((uint16_t)arr[pos] << 8) | ((uint16_t)arr[pos + 1] << 0);
}
uint32_t aread_u32be(uint8_t* arr, size_t pos) {
    return ((uint32_t)arr[pos] << 24) | ((uint32_t)arr[pos + 1] << 16)
        | ((uint32_t)arr[pos + 2] << 8) | ((uint32_t)arr[pos + 3] << 0);
}

String read_str0(vector<uint8_t>& vec, size_t pos) {
    vector<uint8_t> str_vec;
    for(size_t i = pos; vec[i] != 0 && i < vec.size(); i++)
        str_vec.push_back(vec[i]);
    String str((const char*)str_vec.data(), str_vec.size());
    vec.erase(vec.begin() + pos, vec.begin() + pos + str_vec.size() + 1);
    return str;
}
void write_str0(vector<uint8_t>& vec, const String& str) {
    for(size_t i = 0; i < str.length(); i++) {
        vec.push_back(str[i]);
    }
    vec.push_back(0);
}