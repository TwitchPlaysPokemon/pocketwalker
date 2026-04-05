#pragma once
#include <cstdint>
#include <functional>


class Peripheral
{
public:
    virtual void Receive(uint8_t data) = 0;
    virtual uint8_t Transmit() = 0;
    virtual void SetPin(uint8_t pin, bool value) {}
    virtual void Reset() {}
    virtual ~Peripheral() = default;
};
