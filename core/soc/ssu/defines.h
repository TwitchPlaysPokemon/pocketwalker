#pragma once
#include <cstdint>

#define SSU_ADDR_PDR1 0xFFD4
#define SSU_ADDR_PDR9 0xFFDC

struct SSMR_t
{
    uint8_t CKS : 3;
    uint8_t : 2;
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
        uint8_t : 2;
        uint8_t ORER : 1;
        uint8_t : 1;
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
        uint8_t : 1;
        uint8_t RSSTP : 1;
        uint8_t RE : 1;
        uint8_t TE : 1;
    };
};