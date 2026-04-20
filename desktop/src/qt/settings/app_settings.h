#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#include "types/audio_settings.h"
#include "types/control_settings.h"
#include "types/emulation_settings.h"
#include "types/general_settings.h"
#include "types/ir_settings.h"

#define MAX_RECENT_ROMS 5

class AppSettings
{
public:
    static AppSettings instance;

    void load();
    void save() const;

    GeneralSettings general;
    EmulationSettings emulation;
    AudioSettings audio;
    ControlSettings controls;
    IRSettings ir;

private:
    AppSettings() = default;
    static std::string settingsPath();
};

inline void to_json(nlohmann::json& j, const AppSettings& s)
{
    j = nlohmann::json{
            {"general", s.general},
            {"ir", s.ir},
            {"audio", s.audio},
            {"controls", s.controls},
            {"emulation", s.emulation},
    };
}

inline void from_json(const nlohmann::json& j, AppSettings& s)
{
    if (j.contains("general")) j.at("general").get_to(s.general);
    if (j.contains("ir")) j.at("ir").get_to(s.ir);
    if (j.contains("audio")) j.at("audio").get_to(s.audio);
    if (j.contains("controls")) j.at("controls").get_to(s.controls);
    if (j.contains("emulation")) j.at("emulation").get_to(s.emulation);
}