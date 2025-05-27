#pragma once
#include <vector>
#include <stdint.h>
using namespace std;

void write_u16be(vector<uint8_t>& vec, uint16_t n);
void write_u32be(vector<uint8_t>& vec, uint32_t n);
uint16_t read_u16be(vector<uint8_t>& vec, size_t pos);
uint32_t read_u32be(vector<uint8_t>& vec, size_t pos);
uint16_t aread_u16be(uint8_t* arr, size_t pos);
uint32_t aread_u32be(uint8_t* arr, size_t pos);
void write_buf(vector<uint8_t>& vec, uint8_t* buf, size_t len);