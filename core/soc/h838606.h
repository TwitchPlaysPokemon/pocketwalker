#pragma once
#include "core/cpu/cpu.h"
#include "interrupts/interrupts.h"
#include "memory/bus.h"
#include "memory/regions/rom.h"
#include "ssu/ssu.h"
#include "timer/timerb1.h"

class H838606
{
public:
    explicit H838606(RomBuffer rom_buffer);

    void Run();

    std::shared_ptr<MemoryBus> memory = nullptr;

    std::shared_ptr<CPU> cpu = nullptr;
    std::shared_ptr<Interrupts> interrupts = nullptr;

    std::shared_ptr<SSU> ssu = nullptr;

    std::shared_ptr<TimerB1> timer_b1 = nullptr;
};
