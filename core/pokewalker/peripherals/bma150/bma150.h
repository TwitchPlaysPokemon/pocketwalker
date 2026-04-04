#pragma once
#include "core/memory/memory.h"
#include "core/soc/ssu/peripheral.h"

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

private:
    Memory<0x80> mem = {};

    BMA150State state = BMA150State::IDLE;
    bool is_reading = false;
    uint8_t register_index = 0;
};
