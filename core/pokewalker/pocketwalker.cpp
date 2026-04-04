#include "pocketwalker.h"

#include "peripherals/bma150/bma150.h"
#include "peripherals/m95512/m95512.h"

PocketWalker::PocketWalker(RomBuffer rom_buffer, EepromBuffer save_buffer)
{
    this->soc = std::make_shared<H838606>(rom_buffer);

    this->bma150 = std::make_shared<BMA150>();
    this->m95512 = std::make_shared<M95512>(save_buffer);
    this->ssd1854 = std::make_shared<SSD1854>();

    this->soc->ssu->RegisterPeripheral(this->bma150, SSU_ADDR_PDR9, 0);
    this->soc->ssu->RegisterPeripheral(this->m95512, SSU_ADDR_PDR1, 2);
    this->soc->ssu->RegisterPeripheral(this->ssd1854, SSU_ADDR_PDR1, 0, {
        {SSU_ADDR_PDR1, 1, SSD1854_PIN_DC}
    });

}

void PocketWalker::Start()
{
    this->soc->Run();
}
