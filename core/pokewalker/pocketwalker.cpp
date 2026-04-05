#include "pocketwalker.h"

#include <chrono>
#include <thread>

#include "core/soc/defines.h"

PocketWalker::PocketWalker(RomBuffer rom_buffer, EepromBuffer save_buffer)
{
    this->soc = std::make_shared<H838606>(rom_buffer);

    this->bma150 = std::make_shared<BMA150>();
    this->m95512 = std::make_shared<M95512>(save_buffer);
    this->ssd1854 = std::make_shared<SSD1854>();
    this->buzzer = std::make_shared<Buzzer>(this->soc->timer_w);

    this->soc->ssu->RegisterPeripheral(this->bma150, SSU_ADDR_PDR9, 0);
    this->soc->ssu->RegisterPeripheral(this->m95512, SSU_ADDR_PDR1, 2);
    this->soc->ssu->RegisterPeripheral(this->ssd1854, SSU_ADDR_PDR1, 0, {
        {SSU_ADDR_PDR1, 1, SSD1854_PIN_DC}
    });
}

void PocketWalker::Start()
{

    constexpr std::chrono::duration<long long, std::nano> CYCLE_DURATION(1'000'000'000LL / PHI_CLK);

    auto next = std::chrono::high_resolution_clock::now();

    this->is_running = true;
    while (this->is_running)
    {
        const uint8_t cycles = soc->Cycle();

        this->buzzer->Cycle(cycles);

        next += CYCLE_DURATION * cycles;
        std::this_thread::sleep_until(next);
    }
}

void PocketWalker::Stop()
{
    this->is_running = false;
}

void PocketWalker::OnSamplePushed(const EventHandlerCallback<BuzzerInformation>& callback)
{
    this->buzzer->OnSamplePushed += callback;
}

void PocketWalker::PressButton(ButtonType button) const
{
    const uint8_t current = soc->memory->Read8(SSU_ADDR_PDRB);
    soc->memory->Write8(SSU_ADDR_PDRB, current | static_cast<uint8_t>(button));
}

void PocketWalker::ReleaseButton(ButtonType button) const
{
    const uint8_t current = soc->memory->Read8(SSU_ADDR_PDRB);
    soc->memory->Write8(SSU_ADDR_PDRB, current & ~static_cast<uint8_t>(button));
}
