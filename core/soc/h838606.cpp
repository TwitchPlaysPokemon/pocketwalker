#include "h838606.h"

#include <array>
#include <cstdint>
#include <memory>
#include <print>
#include <thread>

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

    rtc = std::make_shared<RTC>(interrupts);
    rtc->RegisterIOHandlers(io);
}

void H838606::Run()
{
    using namespace std::chrono;
    using clock = high_resolution_clock;

    constexpr long long CLK_HZ = 3'686'400;
    constexpr duration<long long, std::nano> CYCLE_DURATION(1'000'000'000LL / CLK_HZ);

    auto next = clock::now();

    while (true)
    {
        const uint8_t cycles = cpu->Cycle();
        interrupts->Cycle(cpu);

        ssu->Cycle(cycles);

        timer_b1->Cycle(cycles);

        rtc->Cycle(cycles);

        next += CYCLE_DURATION * cycles;
        std::this_thread::sleep_until(next);
    }
}
