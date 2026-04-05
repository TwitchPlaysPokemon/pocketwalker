#include "h838606.h"

#include <array>
#include <cstdint>
#include <memory>
#include <print>
#include <thread>

#include "defines.h"
#include "core/utils/logger.h"

H838606::H838606(RomBuffer rom_buffer)
{
    auto rom = std::make_shared<ROM>(rom_buffer);
    auto ram = std::make_shared<RAM>();
    auto io = std::make_shared<IO>();

    memory = std::make_shared<MemoryBus>(rom, ram, io);
    cpu = std::make_shared<CPU>(memory);

    interrupts = std::make_shared<Interrupts>();
    interrupts->RegisterIOHandlers(io);

    ssu = std::make_shared<SSU>(memory, interrupts);
    ssu->RegisterIOHandlers(io);

    timer_b1 = std::make_shared<TimerB1>(interrupts);
    timer_b1->RegisterIOHandlers(io);

    timer_w = std::make_shared<TimerW>(memory, interrupts);
    timer_w->RegisterIOHandlers(io);

    rtc = std::make_shared<RTC>(interrupts);
    rtc->RegisterIOHandlers(io);
}

uint8_t H838606::Cycle()
{
    const uint8_t cycles = cpu->Cycle();
    interrupts->Cycle(cpu);
    ssu->Cycle(cycles);
    timer_b1->Cycle(cycles);
    timer_w->Cycle(cycles);
    rtc->Cycle(cycles);

    return cycles;
}
