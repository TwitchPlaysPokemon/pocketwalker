#include "app_settings.h"
#include <fstream>
#include <QStandardPaths>
#include <QDir>

AppSettings AppSettings::instance = {};

std::string AppSettings::settingsPath()
{
    return "./settings.json";
}

void AppSettings::load()
{
    std::ifstream settings_file(settingsPath());
    if (!settings_file.is_open())
        return;

    nlohmann::json::parse(settings_file).get_to(*this);
}

void AppSettings::save() const
{
    std::ofstream settings_file(settingsPath());
    settings_file << nlohmann::json(*this).dump(4);
}
