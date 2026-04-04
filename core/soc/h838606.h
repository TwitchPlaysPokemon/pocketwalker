#pragma once
#include "core/cpu/cpu.h"
#include "memory/bus.h"
#include "memory/regions/rom.h"
#include "ssu/ssu.h"

class H838606
{
public:
    explicit H838606(RomBuffer rom_buffer);

    void Run();

    std::shared_ptr<MemoryBus> memory = nullptr;
    std::shared_ptr<CPU> cpu = nullptr;
    std::shared_ptr<SSU> ssu = nullptr;
};
