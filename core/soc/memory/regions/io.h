#pragma once
#include <array>

#include "core/memory/interface.h"

#define IO_LOW_SIZE 0xE0
#define IO_LOW_START 0xF020
#define IO_LOW_END 0xF100

#define IO_HIGH_SIZE 0xE0
#define IO_HIGH_START 0xFF80
#define IO_HIGH_END 0xFFFF

class IO : public MemoryInterface
{
public:
    uint8_t Read(uint16_t address) override;
    void Write(uint16_t address, uint8_t value) override;

private:
    std::array<uint8_t, IO_LOW_SIZE> io_low = {};
    std::array<uint8_t, IO_HIGH_SIZE> io_high = {};
};