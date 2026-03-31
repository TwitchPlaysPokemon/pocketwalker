#include "io.h"

uint8_t IO::Read(uint16_t address)
{
    if (address >= IO_LOW_START && address <= IO_LOW_END)
    {
        return this->io_low[address - IO_LOW_START];
    }

    if (address >= IO_HIGH_START && address <= IO_HIGH_END)
    {
        return this->io_high[address - IO_HIGH_START];
    }

    return 0xFF;
}

void IO::Write(uint16_t address, uint8_t value)
{
    if (address >= IO_LOW_START && address <= IO_LOW_END)
    {
        this->io_low[address - IO_LOW_START] = value;
        return;
    }

    if (address >= IO_HIGH_START && address <= IO_HIGH_END)
    {
        this->io_high[address - IO_HIGH_START] = value;
        return;
    }
}
