#pragma once
#include <string>
#include <Qt>
#include <nlohmann/json.hpp>

struct ControlSettings
{
    int key_left = Qt::Key_Left;
    int key_right = Qt::Key_Right;
    int key_center = Qt::Key_Down;
    int key_step = Qt::Key_Up;
    int key_speedup = Qt::Key_Tab;
};

inline void to_json(nlohmann::json& j, const ControlSettings& s)
{
    j = nlohmann::json{
        {"key_left", s.key_left},
        {"key_right", s.key_right},
        {"key_center", s.key_center},
        {"key_step", s.key_step},
        {"key_speedup", s.key_speedup}
    };
}

inline void from_json(const nlohmann::json& j, ControlSettings& s)
{
    s.key_left = j.value("key_left", static_cast<int>(Qt::Key_Left));
    s.key_right = j.value("key_right", static_cast<int>(Qt::Key_Right));
    s.key_center = j.value("key_center", static_cast<int>(Qt::Key_Down));
    s.key_step = j.value("key_step", static_cast<int>(Qt::Key_Up));
    s.key_speedup = j.value("key_speedup", static_cast<int>(Qt::Key_Tab));
}
