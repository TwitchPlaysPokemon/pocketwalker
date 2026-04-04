#include "ssd1854.h"

#include <print>

void SSD1854::Receive(uint8_t data)
{
    if (is_data_mode)
    {

        const uint16_t address = (page * SSD1854_TOTAL_COLUMNS * SSD1854_COLUMN_SIZE) + (column * SSD1854_COLUMN_SIZE) + offset;
        vram.Write8(address, data);

        if (offset == 1)
        {
            column++;
        }

        offset++;
        offset %= 2;
    }
    else
    {
        switch (state) {
        case SSD1854State::IDLE:
            HandleCommand(data);
            break;
        case SSD1854State::SET_CONTRAST:
            contrast = data;
            state = SSD1854State::IDLE;
            break;
        case SSD1854State::SET_PAGE_OFFSET:
            break;
        }

    }
}

uint8_t SSD1854::Transmit()
{
    return 0xFF;
}

void SSD1854::SetPin(uint8_t pin, bool value)
{
    if (pin == SSD1854_PIN_DC)
    {
        is_data_mode = value;
    }
}

void SSD1854::HandleCommand(uint8_t data)
{
    if (data >= SSD1854_CMD_COL_LOW_MIN && data <= SSD1854_CMD_COL_LOW_MAX)
    {
        column = (column & 0xF0) | (data & 0x0F);
        offset = 0;
        state = SSD1854State::IDLE;
    }
    else if (data >= SSD1854_CMD_COL_HIGH_MIN && data <= SSD1854_CMD_COL_HIGH_MAX)
    {
        column = (column & 0xF0) | (data & 0x0F);
        offset = 0;
        state = SSD1854State::IDLE;
    }
    else if (data == SSD1854_CMD_SET_CONTRAST)
    {
        state = SSD1854State::SET_CONTRAST;
    }
    else
    {
        std::println("Invalid LCD Command: 0x{:02X}", data);
    }

}
