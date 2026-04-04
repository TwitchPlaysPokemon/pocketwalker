#pragma once
#include <array>
#include <cstdint>

#define REG_SP_INDEX 7

#define NEGATIVE_MASK(bits) (1 << (bits - 1))

typedef union flags
{
    uint8_t CCR = 0b10000000; // interrupt flag is set by default
    struct
    {
        bool C : 1;
        bool V : 1;
        bool Z : 1;
        bool N : 1;
        bool U : 1;
        bool H : 1;
        bool UI : 1;
        bool I : 1;
    };
} flags_t;

class Registers
{
public:
    Registers();

    uint8_t* Reg8(uint8_t select);
    uint16_t* Reg16(uint8_t select);
    uint32_t* Reg32(uint8_t select);

    template <typename T>
    void MovFlags(T value, const size_t bits = sizeof(T) * 8)
    {
        const uint32_t negative_mask = NEGATIVE_MASK(bits);

        flags.N = value & negative_mask;
        flags.Z = value == 0;
        flags.V = false;
    }

    template <typename T>
    T Add(T rd, T rs, const size_t bits = sizeof(T) * 8)
    {
        const uint32_t negative_mask = NEGATIVE_MASK(bits);
        const uint32_t max_value = (1 << bits) - 1;
        const uint32_t result = rd + rs;

        flags.Z = result == 0;
        flags.N = result & negative_mask;
        flags.V = ((rd ^ rs) & (rd ^ result) & negative_mask) != 0;
        flags.C = result > max_value;
        flags.H = ((rd ^ rs ^ result) >> (bits / 2 - 1) & 1) != 0;

        return static_cast<T>(result);
    }


    template <typename T>
    T Sub(T rd, T rs, const size_t bits = sizeof(T) * 8)
    {
        const uint32_t negative_mask = NEGATIVE_MASK(bits);
        const uint32_t result = rd - rs;

        flags.Z = result == 0;
        flags.N = result & negative_mask;
        flags.V = ((rd ^ rs) & (rd ^ result) & negative_mask) != 0;
        flags.C = rs > rd;
        flags.H = ((rd ^ rs ^ result) >> (bits / 2 - 1) & 1) != 0;

        return static_cast<T>(result);
    }

    template <typename T>
    T Inc(T value, size_t inc, const size_t bits = sizeof(T) * 8)
    {
        const uint32_t negative_mask = NEGATIVE_MASK(bits);
        const uint32_t result = static_cast<uint32_t>(value) + inc;

        flags.N = result & negative_mask;
        flags.Z = result == 0;
        flags.V = value == (negative_mask >> 1) - 1;

        return static_cast<T>(result);
    }

    template <typename T>
    T Dec(T value, size_t dec, const size_t bits = sizeof(T) * 8)
    {
        const uint32_t negative_mask = NEGATIVE_MASK(bits);
        const uint32_t result = static_cast<uint32_t>(value) - dec;

        flags.N = result & negative_mask;
        flags.Z = result == 0;
        flags.V = value == negative_mask;

        return static_cast<T>(result);
    }


    uint16_t PC;
    uint32_t* SP;
    flags_t flags;

private:
    std::array<uint32_t, 8> registers = {};
};
