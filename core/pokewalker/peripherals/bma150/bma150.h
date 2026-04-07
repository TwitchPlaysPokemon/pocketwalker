#pragma once
#include "core/memory/memory.h"
#include "core/soc/ssu/peripheral.h"

#define BMA150_PIN_INT 4

#define BMA150_ADDR_RANGE_BANDWIDTH_REG 0x14

enum class BMA150State
{
    IDLE,
    MEMORY
};

class BMA150 : public Peripheral
{
public:
    void Receive(uint8_t data) override;
    uint8_t Transmit() override;
    void Cycle(uint32_t cycles) override;

private:
    Memory<0x80> mem = {};

    BMA150State state = BMA150State::IDLE;
    bool is_reading = false;
    uint8_t register_index = 0;
    uint32_t cycle_count = 0;
};