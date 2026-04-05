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

    IO_HANDLER_READ_UNION(INTERRUPT_ADDR_RTCFLG, RTCFLG);
    IO_HANDLER_WRITE_UNION(INTERRUPT_ADDR_RTCFLG, RTCFLG);
}

void Interrupts::Cycle(const std::shared_ptr<CPU>& cpu)
{
    if (cpu->reg.flags.I)
        return;

    if (IENR1.IEN0 && IRR1.IRRI0)
    {
        cpu->Interrupt(INTERRUPT_VECTOR_ADDR_IRQ0);
    }
    else if (IENR1.IENRTC && RTCFLG.SEIFG025)
    {
        cpu->Interrupt(INTERRUPT_VECTOR_ADDR_RTC_QUARTER_SEC);
    }
    else if (IENR1.IENRTC && RTCFLG.SEIFG05)
    {
        cpu->Interrupt(INTERRUPT_VECTOR_ADDR_RTC_HALF_SEC);
    }
    else if (IENR1.IENRTC && RTCFLG.SEIFG1)
    {
        cpu->Interrupt(INTERRUPT_VECTOR_ADDR_RTC_SEC);
    }
    else if (IENR2.IENTB1 && IRR2.IRRTB1)
    {
        cpu->Interrupt(INTERRUPT_VECTOR_ADDR_TIMER_B1);
    }
}
