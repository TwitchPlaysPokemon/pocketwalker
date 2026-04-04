#pragma once
#include "core/memory/memory.h"
#include "core/soc/ssu/peripheral.h"

#define SSD1854_PIN_DC 0

#define SSD1854_CMD_COL_LOW_MIN 0x0
#define SSD1854_CMD_COL_LOW_MAX 0xF
#define SSD1854_CMD_COL_HIGH_MIN 0x10
#define SSD1854_CMD_COL_HIGH_MAX 0x17

#define SSD1854_CMD_SET_CONTRAST 0x81


#define SSD1854_COLUMN_SIZE 2
#define SSD1854_TOTAL_COLUMNS 128

enum class SSD1854State
{
    IDLE,
    SET_CONTRAST,
    SET_PAGE_OFFSET
};

class SSD1854 : public Peripheral
{
public:
    void Receive(uint8_t data) override;
    uint8_t Transmit() override;
    void SetPin(uint8_t pin, bool value) override;

private:
    void HandleCommand(uint8_t data);

    Memory<0xA00> vram = {};

    SSD1854State state = SSD1854State::IDLE;

    uint8_t column = 0;
    uint8_t offset = 0;
    uint8_t page = 0;
    uint8_t contrast = 20;
    uint8_t page_offset = 0;
    bool power_save_mode = false;

    bool is_data_mode = false;
};

