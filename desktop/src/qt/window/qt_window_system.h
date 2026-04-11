#pragma once
#include <memory>
#include <thread>
#include <string>
#include <optional>
#include <QMainWindow>
#include <QKeyEvent>
#include <QTimer>
#include <QThread>
#include <QSettings>
#include "core/pokewalker/pocketwalker.h"
#include "desktop/src/qt/audio/qt_audio_system.h"
#include "desktop/src/qt/network/qt_network_system.h"

class DisplayWidget;

#define MAX_RECENT_ROMS 5

class QtWindowSystem : public QMainWindow
{
    Q_OBJECT
public:
    explicit QtWindowSystem(QWidget* parent = nullptr);
    ~QtWindowSystem() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void openROM();
    void openRecentROM(const QString& path);
    void importSave();
    void resetEmulator();

private:
    void launchEmulator(const std::string& rom_path);
    void shutdownEmulator();
    void addToRecentROMs(const QString& path);
    void updateRecentROMsMenu();

    std::optional<PocketWalker> emulator;
    std::unique_ptr<QtAudioSystem> audio;
    std::unique_ptr<QtNetworkSystem> network;
    std::unique_ptr<QThread> network_thread;
    std::unique_ptr<std::thread> emulator_thread;

    DisplayWidget* display = nullptr;
    QTimer* render_timer = nullptr;
    QMenu* recent_roms_menu = nullptr;
    QAction* import_save_action = nullptr;
    QAction* reset_action = nullptr;

    std::string save_path;
    std::string active_rom_path;
};