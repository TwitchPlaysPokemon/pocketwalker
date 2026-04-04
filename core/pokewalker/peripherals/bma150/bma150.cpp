#include "bma150.h"

void BMA150::Receive(uint8_t data)
{
    switch (state)
    {
    case BMA150State::IDLE:
        is_reading = data & 0b10000000;
        register_index = data & 0b01111111;
        state = BMA150State::MEMORY;
        break;
    case BMA150State::MEMORY:
        if (!is_reading)
            mem.Write8(register_index, data);

        state = BMA150State::IDLE;
        break;
    }
}

uint8_t BMA150::Transmit()
{
    if (is_reading)
        return mem.Read8(register_index);

    state = BMA150State::IDLE;

    return 0xFF;
}
