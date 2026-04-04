#pragma once

#include <cstdint>

class MemoryInterface
{
public:
    virtual ~MemoryInterface() = default;

    uint8_t Read8(uint16_t address);
    uint16_t Read16(uint16_t address);
    uint32_t Read32(uint16_t address);

    void Write8(uint16_t address, uint8_t value);
    void Write16(uint16_t address, uint16_t value);
    void Write32(uint16_t address, uint32_t value);

    uint8_t* Ptr8(uint16_t address);
    uint16_t* Ptr16(uint16_t address);
    uint32_t* Ptr32(uint16_t address);


protected:
    virtual uint8_t Read(uint16_t address) = 0;
    virtual void Write(uint16_t address, uint8_t value) = 0;
    virtual void* Ptr(uint16_t address) = 0;
};