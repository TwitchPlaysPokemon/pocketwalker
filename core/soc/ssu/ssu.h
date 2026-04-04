#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "defines.h"
#include "peripheral.h"
#include "core/memory/interface.h"
#include "core/soc/memory/regions/io.h"

#define SSU_ADDR_SSMR 0xF0E2
#define SSU_ADDR_SSER 0xF0E3
#define SSU_ADDR_SSSR 0xF0E4
#define SSU_ADDR_SSRDR 0xF0E9
#define SSU_ADDR_SSTDR 0xF0EB

struct SSUPeripheralPinEntry
{
    uint16_t port_addr;
    uint8_t pin_index;
    uint8_t peripheral_pin;
};

struct SSUPeripheralEntry
{
    std::shared_ptr<Peripheral> peripheral;
    uint16_t port_addr;
    uint8_t pin_index;
    std::vector<SSUPeripheralPinEntry> pins;
};

class SSU
{
public:
    explicit SSU(const std::shared_ptr<MemoryInterface>& memory);

    void RegisterIOHandlers(const std::shared_ptr<IO>& io);

    void RegisterPeripheral(const std::shared_ptr<Peripheral>& peripheral, uint16_t port_addr, uint8_t pin_index, const std::vector<
                            SSUPeripheralPinEntry>& pins = {});

    void Cycle(uint8_t cycles);

    SSMR_t* SSMR = nullptr;
    SSER_t SSER = {};
    SSSR_t SSSR = {};
    uint8_t SSRDR = 0;
    uint8_t SSTDR = 0;

private:
    std::shared_ptr<Peripheral> ActivePeripheral();

    std::shared_ptr<MemoryInterface> mem = nullptr;

    std::vector<SSUPeripheralEntry> peripherals;

    uint32_t ssu_cycles = 0;
};
