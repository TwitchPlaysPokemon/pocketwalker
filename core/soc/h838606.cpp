#include "h838606.h"

#include <array>
#include <cstdint>
#include <memory>
#include <print>

H838606::H838606(RomBuffer rom_buffer)
{
    auto rom = std::make_shared<ROM>(rom_buffer);
    auto ram = std::make_shared<RAM>();
    auto io = std::make_shared<IO>();

    memory = std::make_shared<MemoryBus>(rom, ram, io);
    cpu = std::make_shared<CPU>(memory);

    interrupts = std::make_shared<Interrupts>();
    interrupts->RegisterIOHandlers(io);

    ssu = std::make_shared<SSU>(memory);
    ssu->RegisterIOHandlers(io);

    timer_b1 = std::make_shared<TimerB1>(interrupts);
    timer_b1->RegisterIOHandlers(io);
}

void H838606::Run()
{
    while (true)
    {
        const uint8_t cycles = cpu->Cycle();
        interrupts->Cycle(cpu);

        ssu->Cycle(cycles);

        timer_b1->Cycle(cycles);
    }
}
