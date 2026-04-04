#include "interrupts.h"

#include "core/cpu/cpu.h"
#include "core/soc/defines.h"

void Interrupts::RegisterIOHandlers(const std::shared_ptr<IO>& io)
{
    IO_HANDLER_READ_UNION(INTERRUPT_ADDR_IENR1, IENR1);
    IO_HANDLER_WRITE_UNION(INTERRUPT_ADDR_IENR1, IENR1);

    IO_HANDLER_READ_UNION(INTERRUPT_ADDR_IENR2, IENR2);
    IO_HANDLER_WRITE_UNION(INTERRUPT_ADDR_IENR2, IENR2);

    IO_HANDLER_READ_UNION(INTERRUPT_ADDR_IRR1, IRR1);
    IO_HANDLER_WRITE_UNION(INTERRUPT_ADDR_IRR1, IRR1);

    IO_HANDLER_READ_UNION(INTERRUPT_ADDR_IRR2, IRR2);
    IO_HANDLER_WRITE_UNION(INTERRUPT_ADDR_IRR2, IRR2);
}

void Interrupts::Cycle(const std::shared_ptr<CPU>& cpu)
{
    if (cpu->reg.flags.I)
        return;

    if (IENR2.IENTB1 && IRR2.IRRTB1)
    {
        cpu->Interrupt(INTERRUPT_VECTOR_ADDR_TIMER_B1);
    }
}
