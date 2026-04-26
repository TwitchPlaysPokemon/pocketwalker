#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct GeneralSettings
{
    enum class AppTheme { System, Light, Dark };

    AppTheme theme = AppTheme::System;
    bool boot_on_launch = false;
    bool prevent_activity_timeout = false;
    std::string default_rom;
    std::vector<std::string> recent_roms;
};

inline void to_json(nlohmann::json& j, const GeneralSettings& s)
{
    j = nlohmann::json{
        {"theme", static_cast<int>(s.theme)},
        {"boot_on_launch", s.boot_on_launch},
        {"default_rom", s.default_rom},
        {"recent_roms", s.recent_roms},
        {"prevent_activity_timeout", s.prevent_activity_timeout}
    };
}

inline void from_json(const nlohmann::json& j, GeneralSettings& s)
{
    s.theme = static_cast<GeneralSettings::AppTheme>(j.value("theme",
        static_cast<int>(GeneralSettings::AppTheme::System)));
    s.boot_on_launch = j.value("boot_on_launch", false);
    s.default_rom = j.value("default_rom", "");
    s.recent_roms = j.value("recent_roms", std::vector<std::string>{});
    s.prevent_activity_timeout = j.value("prevent_activity_timeout", false);
}
