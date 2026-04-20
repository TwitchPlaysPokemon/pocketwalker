#pragma once
#include <array>
#include <cstdint>
#include <nlohmann/json.hpp>

struct AudioSettings
{
    float volume = 1.0f;
};

inline void to_json(nlohmann::json& j, const AudioSettings& s)
{
    j = nlohmann::json{
        {"volume", s.volume}
    };
}

inline void from_json(const nlohmann::json& j, AudioSettings& s)
{
    s.volume = j.value("volume", 1.0f);
}
