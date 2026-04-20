#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct IRSettings
{
    enum class Mode { Client, Server };

    Mode mode = Mode::Client;
    std::string host = "127.0.0.1";
    int port = 8081;
};

inline void to_json(nlohmann::json& j, const IRSettings& s)
{
    j = nlohmann::json{
        {"mode", static_cast<int>(s.mode)},
        {"host", s.host},
        {"port", s.port}
    };
}

inline void from_json(const nlohmann::json& j, IRSettings& s)
{
    s.mode = static_cast<IRSettings::Mode>(j.value("mode", static_cast<int>(IRSettings::Mode::Server)));
    s.host = j.value("host", "127.0.0.1");
    s.port = j.value("port", 5000);
}
