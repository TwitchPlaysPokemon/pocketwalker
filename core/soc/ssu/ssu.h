#pragma once
#include <memory>
#include <unordered_map>
#include <vector>

#include "peripheral.h"
#include "core/memory/interface.h"
#include "core/soc/interrupts/interrupts.h"
#include "core/soc/memory/regions/io.h"

#define SSU_ADDR_SSMR 0xF0E2
#define SSU_ADDR_SSER 0xF0E3
#define SSU_ADDR_SSSR 0xF0E4
#define SSU_ADDR_SSRDR 0xF0E9
#define SSU_ADDR_SSTDR 0xF0EB

#define SSU_ADDR_PDR1 0xFFD4
#define SSU_ADDR_PDR9 0xFFDC
#define SSU_ADDR_PDRB 0xFFDE

#define SSU_ADDR_PMRB 0xFFCA

struct SSMR_t
{
    uint8_t CKS : 3;
    uint8_t  : 2;
    uint8_t CPHS : 1;
    uint8_t CPOS : 1;
    uint8_t MLS : 1;
};

union SSSR_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t CE : 1;
        uint8_t RDRF : 1;
        uint8_t TDRE : 1;
        uint8_t TEND : 1;
        uint8_t  : 2;
        uint8_t ORER : 1;
        uint8_t  : 1;
    };
};

union SSER_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t CEIE : 1;
        uint8_t RIE : 1;
        uint8_t TIE : 1;
        uint8_t TEIE : 1;
        uint8_t  : 1;
        uint8_t RSSTP : 1;
        uint8_t RE : 1;
        uint8_t TE : 1;
    };
};

union PDRB_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t PB0 : 1;
        uint8_t PB1 : 1;
        uint8_t PB2 : 1;
        uint8_t PB3 : 1;
        uint8_t PB4 : 1;
        uint8_t PB5 : 1;
        uint8_t  : 2;
    };
};


union PMRB_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t IRQ0 : 1;
        uint8_t IRQ1 : 1;
        uint8_t  : 2;
        uint8_t ADTSTCHG : 1;
        uint8_t  : 4;
    };
};


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
    explicit SSU(const std::shared_ptr<MemoryInterface>& memory, const std::shared_ptr<Interrupts>& interrupts);

    void RegisterIOHandlers(const std::shared_ptr<IO>& io);

    void RegisterPeripheral(const std::shared_ptr<Peripheral>& peripheral, uint16_t port_addr, uint8_t pin_index,
                            const std::vector<
                                SSUPeripheralPinEntry>& pins = {});

    void Cycle(uint8_t cycles);

    PDRB_t PDRB = {};
    PMRB_t PMRB = {};

    SSMR_t* SSMR = nullptr;
    SSER_t SSER = {};
    SSSR_t SSSR = {};
    uint8_t SSRDR = 0;
    uint8_t SSTDR = 0;

private:
    std::shared_ptr<Peripheral> ActivePeripheral();

    std::shared_ptr<MemoryInterface> mem = nullptr;
    std::shared_ptr<Interrupts> interrupts = nullptr;

    std::vector<SSUPeripheralEntry> peripherals;

    uint32_t ssu_cycles = 0;
};
