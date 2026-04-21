#include "emulator_context.h"
#include <fstream>
#include <filesystem>
#include "desktop/src/qt/settings/app_settings.h"

EmulatorContext::EmulatorContext(const std::string& rom_path, const std::string& save_path, QObject* parent)
    : QObject(parent), rom_path(rom_path)
{
    this->save_path = save_path;

    RomBuffer rom_buffer = {};
    std::ifstream rom_file(rom_path, std::ios::binary);
    rom_file.read(reinterpret_cast<char*>(rom_buffer.data()), 0xC000);

    emu.emplace(rom_buffer);
    loadSave();

    audio = std::make_unique<QtAudioSystem>();
    emu->OnSamplePushed([this](BuzzerInformation info)
    {
        audio->PushSample(info);
    });

    const auto& ir = AppSettings::instance.ir;

    network_thread = std::make_unique<QThread>();
    network = std::make_unique<QtNetworkSystem>(
        *emu,
        ir.mode == IRSettings::Mode::Server,
        QString::fromStdString(ir.host),
        static_cast<quint16>(ir.port),
        5
    );
    network->moveToThread(network_thread.get());
    connect(network_thread.get(), &QThread::started, network.get(), &QtNetworkSystem::start);
    network_thread->start();

    emulator_thread = std::make_unique<std::thread>([this] { emu->Start(); });
}

EmulatorContext::EmulatorContext(const std::string& path, QObject* parent)
    : EmulatorContext(path, path.substr(0, path.find_last_of('.')) + ".sav", parent)
{

}

EmulatorContext::~EmulatorContext()
{
    emu->Stop();
    if (emulator_thread && emulator_thread->joinable())
        emulator_thread->join();

    if (network_thread)
    {
        network_thread->quit();
        network_thread->wait();
    }

    writeSave();
}

void EmulatorContext::loadSave()
{
    if (!std::filesystem::exists(save_path))
        return;

    EepromBuffer buf = {};
    std::ifstream f(save_path, std::ios::binary);
    f.read(reinterpret_cast<char*>(buf.data()), buf.size());
    emu->SetEepromBuffer(buf);
}

void EmulatorContext::writeSave()
{
    if (save_path.empty())
        return;

    const EepromBuffer buf = emu->GetEepromBuffer();
    std::ofstream f(save_path, std::ios::binary);
    f.write(reinterpret_cast<const char*>(buf.data()), buf.size());
}