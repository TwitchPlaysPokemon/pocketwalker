#pragma once
#include <memory>

#include "core/cpu/cpu.h"
#include "core/soc/memory/regions/io.h"

#define INTERRUPT_ADDR_IEGR 0xFFF2
#define INTERRUPT_ADDR_IENR1 0xFFF3
#define INTERRUPT_ADDR_IENR2 0xFFF4

#define INTERRUPT_ADDR_IRR1 0xFFF6
#define INTERRUPT_ADDR_IRR2 0xFFF7

#define INTERRUPT_VECTOR_ADDR_TIMER_B1 0x0042

union IENR1_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t IEN0 : 1;
        uint8_t IEN1 : 1;
        uint8_t IENEC2 : 1;
        uint8_t  : 4;
        uint8_t IENRTC : 1;
    };
};

union IENR2_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t IENEC : 1;
        uint8_t  : 1;
        uint8_t IENTB1 : 1;
        uint8_t  : 3;
        uint8_t IENAD : 1;
        uint8_t  : 1;
    };
};


union IRR1_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t IRRI0 : 1;
        uint8_t IRRI1 : 1;
        uint8_t IRREC2 : 1;
        uint8_t  : 5;
    };
};

union IRR2_t
{
    uint8_t VALUE;

    struct
    {
        uint8_t IRREC : 1;
        uint8_t  : 1;
        uint8_t IRRTB1 : 1;
        uint8_t  : 3;
        uint8_t IRRAD : 1;
        uint8_t  : 1;
    };
};

class Interrupts
{
public:
    void RegisterIOHandlers(const std::shared_ptr<IO>& io);

    void Cycle(const std::shared_ptr<CPU>& cpu);

    IENR1_t IENR1 = {};
    IENR2_t IENR2 = {};
    IRR1_t IRR1 = {};
    IRR2_t IRR2 = {};
};
