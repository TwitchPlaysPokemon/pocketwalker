#include "cpu.h"

#include <print>

#include "instructions/set.h"

static InstructionSet instruction_set = {};

CPU::CPU(std::shared_ptr<MemoryInterface> memory)
{
    this->mem = memory;

    this->reg.PC = this->mem->Read16(CPU_VECTOR_RESET_ADDR);
}

uint8_t CPU::Cycle()
{
    uint16_t exec_pc = this->reg.PC;
    const Instruction* instruction = instruction_set.Decode(*this);

    instruction->execute(*this);

    if (!instruction->branch_absolute)
        this->reg.PC += instruction->size;

    //std::println("0x{:04X} {}", exec_pc, instruction->name);

    return instruction->cycles.Count();
}

void CPU::Push16(uint16_t value)
{
    *this->reg.SP -= 2;
    this->mem->Write16(*this->reg.SP, value);
}

uint16_t CPU::Pop16()
{
    const uint16_t value = this->mem->Read16(*this->reg.SP);
    *this->reg.SP += 2;

    return value;
}
