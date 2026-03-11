#pragma once
#define CRC32_SEED 0xFFFFFFFFU
uint32_t GetCRC32(std::vector<uint8_t>& Data, uint32_t SeedValue);