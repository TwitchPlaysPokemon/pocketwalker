#include <fstream>
#include <print>
#include <thread>
#include <QApplication>

#include "argparse/argparse.hpp"
#include "core/pokewalker/pocketwalker.h"
#include "qt/main_window.h"

int main(int argc, char* argv[])
{
    argparse::ArgumentParser arguments("pocketwalker");

    arguments.add_argument("rom")
        .help("The path to the PokeWalker rom file.")
        .default_value("rom.bin");

    arguments.add_argument("save")
        .help("The path to your PokeWalker save file.")
        .default_value("pocketwalker.sav");

    try {
        arguments.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arguments;

        std::println("Press any key to exit...");
        std::cin.get();
        return 1;
    }

    auto rom_path = arguments.get<std::string>("rom");
    RomBuffer rom_buffer = {};
    if (std::filesystem::exists(rom_path))
    {
        std::ifstream rom_file(rom_path, std::ios::binary);
        rom_file.read(reinterpret_cast<char*>(rom_buffer.data()), 0xC000);
        rom_file.close();
    }
    else
    {
        std::println("Failed to find a rom with the name \"{}\"", rom_path);
        std::cin.get();
        return 1;
    }

    auto save_path = arguments.get<std::string>("save");
    EepromBuffer save_buffer = {};
    if (std::filesystem::exists(save_path))
    {
        std::ifstream eeprom_file(save_path, std::ios::binary);
        eeprom_file.read(reinterpret_cast<char*>(save_buffer.data()), save_buffer.size());
        eeprom_file.close();
    }

    PocketWalker emulator(rom_buffer, save_buffer);
    std::thread emulator_thread([&]{ emulator.Start(); });

    QApplication app(argc, argv);
    app.setStyle("Fusion");

    MainWindow window(emulator);
    window.show();

    return app.exec();
}
