#pragma once
#include <array>
#include <cstdint>
#include <memory>

#include "core/soc/h838606.h"
#include "peripherals/bma150/bma150.h"
#include "peripherals/m95512/m95512.h"
#include "peripherals/ssd1854/ssd1854.h"

enum class ButtonType
{
    CENTER = 1 << 0,
    LEFT = 1 << 2,
    RIGHT = 1 << 4
};

class PocketWalker
{
public:
    PocketWalker(RomBuffer rom_buffer, EepromBuffer save_buffer);

    void Start();

    void PressButton(ButtonType button) const;
    void ReleaseButton(ButtonType button) const;

    std::shared_ptr<SSD1854> ssd1854 = nullptr;

private:
    std::shared_ptr<H838606> soc = nullptr;

    std::shared_ptr<BMA150> bma150 = nullptr;
    std::shared_ptr<M95512> m95512 = nullptr;
};
