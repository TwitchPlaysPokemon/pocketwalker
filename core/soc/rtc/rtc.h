#pragma once
#include "core/soc/interrupts/interrupts.h"


class RTC
{
public:
    explicit RTC(const std::shared_ptr<Interrupts>& interrupts);

    void RegisterIOHandlers(const std::shared_ptr<IO>& io);

    void Cycle(uint8_t cycles);

private:
    std::shared_ptr<Interrupts> interrupts;

    uint32_t rtc_cycles = 0;

    uint8_t quarters = 0;
};
