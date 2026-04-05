#include "rtc.h"

#include "core/soc/defines.h"

RTC::RTC(const std::shared_ptr<Interrupts>& interrupts)
{
    this->interrupts = interrupts;
}

void RTC::RegisterIOHandlers(const std::shared_ptr<IO>& io)
{
}

void RTC::Cycle(uint8_t cycles)
{
    rtc_cycles += cycles;
    if (rtc_cycles >= PHI_CLK / 4)
    {
        rtc_cycles -= PHI_CLK / 4;

        quarters++;

        interrupts->RTCFLG.SEIFG025 = true;

        if (quarters % 2 == 0)
        {
            interrupts->RTCFLG.SEIFG05 = true;
        }

        if (quarters % 4 == 0)
        {
            interrupts->RTCFLG.SEIFG1 = true;
            quarters = 0; // only track quarters for second increments
        }

        // TODO sec/min/hr/day registers
        // TODO add other rtc interrupts for min/hr/day/free-running
    }
}
