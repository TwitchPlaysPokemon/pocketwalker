#include "m95512.h"

#include <print>

M95512::M95512(EepromBuffer eeprom_buffer)
{
    eeprom = eeprom_buffer;
}

void M95512::Receive(uint8_t data)
{
    switch (state)
    {
    case M95512State::IDLE:

        if (data == M95512_CMD_WRITE_MEMORY)
        {
            is_reading = false;
            state = M95512State::ADDR_HIGH;
        }
        else if (data == M95512_CMD_READ_MEMORY)
        {
            is_reading = true;
            state = M95512State::ADDR_HIGH;
        }
        else if (data == M95512_CMD_READ_STATUS)
        {
            state = M95512State::STATUS;
        }
        else if (data == M95512_CMD_WRITE_ENABLE)
        {
            status |= M95512_STATUS_WRITE_UNLOCK;
            state = M95512State::IDLE;
        }
        return;
    case M95512State::ADDR_HIGH:
        high_addr = data;
        state = M95512State::ADDR_LOW;
        return;
    case M95512State::ADDR_LOW:
        low_addr = data;
        state = M95512State::MEMORY;
        return;
    case M95512State::MEMORY:
        if (!is_reading)
        {
            eeprom[(high_addr << 8 | low_addr) + offset] = data;
            offset++;
            offset %= 128;
        }

        state = M95512State::IDLE;
    }
}

uint8_t M95512::Transmit()
{
    switch (state)
    {
    case M95512State::MEMORY:
        {
            const uint8_t value = is_reading ? eeprom[(high_addr << 8 | low_addr) + offset] : 0xFF;
            offset++;
            offset %= 128;

            state = M95512State::IDLE;
            return value;
        }
    case M95512State::STATUS:
        state = M95512State::IDLE;
        return status;
    default:
        return 0xFF;
    }
}
