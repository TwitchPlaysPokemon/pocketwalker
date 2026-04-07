#include "bma150.h"

#include "core/soc/defines.h"

static std::array<uint16_t, 8> clock_rates = {
    50, 100, 190, 375, 750, 1500, 3000, 3000
};

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

void BMA150::Cycle(uint32_t cycles)
{
    const uint16_t clock_rate = clock_rates[mem.Read8(BMA150_ADDR_RANGE_BANDWIDTH_REG) & 0b111];

    cycle_count += cycles;
    if (cycle_count >= PHI_CLK / clock_rate)
    {
        cycle_count -= PHI_CLK / clock_rate;

        // trigger an interrupt for "data receive"
        OnOutputPin({BMA150_PIN_INT, true});
        OnOutputPin({BMA150_PIN_INT, false});
    }
}