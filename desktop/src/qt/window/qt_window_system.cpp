#include "qt_window_system.h"
#include "widgets/display_widget.h"
#include <fstream>
#include <QMenuBar>
#include <QTimer>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

QtWindowSystem::QtWindowSystem(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("PocketWalker");
    menuBar()->setNativeMenuBar(true);

    auto* file_menu = menuBar()->addMenu("File");
    auto* open_action = file_menu->addAction("Open ROM");
    open_action->setShortcut(QKeySequence::Open);
    connect(open_action, &QAction::triggered, this, &QtWindowSystem::openROM);

    recent_roms_menu = file_menu->addMenu("Recent ROMs");
    updateRecentROMsMenu();

    file_menu->addSeparator();
    import_save_action = file_menu->addAction("Import Save");
    import_save_action->setEnabled(false);
    connect(import_save_action, &QAction::triggered, this, &QtWindowSystem::importSave);

    reset_action = file_menu->addAction("Reset");
    reset_action->setEnabled(false);
    connect(reset_action, &QAction::triggered, this, &QtWindowSystem::resetEmulator);

    file_menu->addSeparator();
    connect(file_menu->addAction("Exit"), &QAction::triggered, qApp, &QApplication::quit);

    auto* settings_menu = menuBar()->addMenu("Settings");
    settings_menu->addAction("General");
    settings_menu->addAction("Emulation");
    settings_menu->addAction("Controls");
    settings_menu->addAction("IR");

    display = new DisplayWidget(this);
    setCentralWidget(display);
    adjustSize();

    render_timer = new QTimer(this);
    connect(render_timer, &QTimer::timeout, display, QOverload<>::of(&QWidget::update));
    render_timer->start(16);
}

QtWindowSystem::~QtWindowSystem()
{
    shutdownEmulator();
}

void QtWindowSystem::openROM()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Open ROM", QString(), "ROM Files (*.bin *.rom);;All Files (*)");

    if (path.isEmpty())
        return;

    launchEmulator(path.toStdString());
}

void QtWindowSystem::openRecentROM(const QString& path)
{
    if (!QFileInfo::exists(path))
    {
        QMessageBox::warning(this, "File Not Found",
            QString("Could not find:\n%1\n\nIt will be removed from the recent list.").arg(path));

        QSettings settings("PocketWalker", "PocketWalker");
        QStringList recent = settings.value("recentROMs").toStringList();
        recent.removeAll(path);
        settings.setValue("recentROMs", recent);
        updateRecentROMsMenu();
        return;
    }

    launchEmulator(path.toStdString());
}

void QtWindowSystem::importSave()
{
    if (!emulator || save_path.empty())
    {
        QMessageBox::warning(this, "No ROM Loaded", "Please load a ROM before importing a save.");
        return;
    }

    const QString path = QFileDialog::getOpenFileName(
        this, "Import Save", QString(), "Save Files (*.sav);;All Files (*)");

    if (path.isEmpty())
        return;

    std::ifstream source_save_file(path.toStdString(), std::ios::binary);
    const std::string current_rom = active_rom_path;

    shutdownEmulator();

    std::ofstream dst(save_path, std::ios::binary);
    dst << source_save_file.rdbuf();
    dst.close();

    launchEmulator(current_rom);
}

void QtWindowSystem::resetEmulator()
{
    if (active_rom_path.empty())
        return;

    launchEmulator(active_rom_path);
}

void QtWindowSystem::addToRecentROMs(const QString& path)
{
    QSettings settings("PocketWalker", "PocketWalker");
    QStringList recent = settings.value("recentROMs").toStringList();

    recent.removeAll(path);
    recent.prepend(path);
    while (recent.size() > MAX_RECENT_ROMS)
        recent.removeLast();

    settings.setValue("recentROMs", recent);
    updateRecentROMsMenu();
}

void QtWindowSystem::updateRecentROMsMenu()
{
    recent_roms_menu->clear();

    const QSettings settings("PocketWalker", "PocketWalker");
    const QStringList recent = settings.value("recentROMs").toStringList();

    if (recent.isEmpty())
    {
        recent_roms_menu->addAction("(empty)")->setEnabled(false);
        return;
    }

    for (const QString& path : recent)
    {
        const QAction* action = recent_roms_menu->addAction(path);
        connect(action, &QAction::triggered, this, [this, path]
        {
            openRecentROM(path);
        });
    }

    recent_roms_menu->addSeparator();
    connect(recent_roms_menu->addAction("Clear Recent ROMs"), &QAction::triggered, this, [this]
    {
        QSettings settings("PocketWalker", "PocketWalker");
        settings.remove("recentROMs");
        updateRecentROMsMenu();
    });
}

void QtWindowSystem::launchEmulator(const std::string& rom_path)
{
    shutdownEmulator();
    addToRecentROMs(QString::fromStdString(rom_path));
    active_rom_path = rom_path;

    RomBuffer rom_buffer = {};
    std::ifstream rom_file(rom_path, std::ios::binary);
    rom_file.read(reinterpret_cast<char*>(rom_buffer.data()), 0xC000);

    emulator.emplace(PocketWalker(rom_buffer));

    save_path = rom_path.substr(0, rom_path.find_last_of('.')) + ".sav";
    if (std::filesystem::exists(save_path))
    {
        EepromBuffer save_buf = {};
        std::ifstream save_file(save_path, std::ios::binary);
        save_file.read(reinterpret_cast<char*>(save_buf.data()), save_buf.size());
        emulator->SetEepromBuffer(save_buf);
    }

    audio = std::make_unique<QtAudioSystem>();
    emulator->OnSamplePushed([this](BuzzerInformation info)
    {
        audio->PushSample(info);
    });

    network_thread = std::make_unique<QThread>();
    network = std::make_unique<QtNetworkSystem>(*emulator, false, "127.0.0.1", 8081, 5);
    network->moveToThread(network_thread.get());
    connect(network_thread.get(), &QThread::started, network.get(), &QtNetworkSystem::start);
    network_thread->start();

    display->setEmulator(&emulator.value());
    import_save_action->setEnabled(true);
    reset_action->setEnabled(true);

    render_timer->start(16);

    setWindowTitle(QString("PocketWalker - %1")
        .arg(QString::fromStdString(rom_path.substr(rom_path.find_last_of("/\\") + 1))));

    emulator_thread = std::make_unique<std::thread>([this] { emulator->Start(); });
}

void QtWindowSystem::shutdownEmulator()
{
    if (!emulator)
        return;

    render_timer->stop();

    emulator->Stop();
    if (emulator_thread && emulator_thread->joinable())
        emulator_thread->join();
    emulator_thread.reset();

    if (network_thread)
    {
        network_thread->quit();
        network_thread->wait();
    }
    network.reset();
    network_thread.reset();
    audio.reset();

    if (!save_path.empty())
    {
        const EepromBuffer buf = emulator->GetEepromBuffer();
        std::ofstream f(save_path, std::ios::binary);
        f.write(reinterpret_cast<const char*>(buf.data()), buf.size());
    }

    emulator.reset();
    display->setEmulator(nullptr);
    import_save_action->setEnabled(false);
    reset_action->setEnabled(false);

    setWindowTitle("PocketWalker");
}

void QtWindowSystem::keyPressEvent(QKeyEvent* event)
{
    if (!emulator)
    {
        QMainWindow::keyPressEvent(event);
        return;
    }
    switch (event->key())
    {
    case Qt::Key_Down: emulator->PressButton(ButtonType::CENTER);
        break;
    case Qt::Key_Left: emulator->PressButton(ButtonType::LEFT);
        break;
    case Qt::Key_Right: emulator->PressButton(ButtonType::RIGHT);
        break;
    default: QMainWindow::keyPressEvent(event);
        break;
    }
}

void QtWindowSystem::keyReleaseEvent(QKeyEvent* event)
{
    if (!emulator)
    {
        QMainWindow::keyReleaseEvent(event);
        return;
    }
    switch (event->key())
    {
    case Qt::Key_Down: emulator->ReleaseButton(ButtonType::CENTER);
        break;
    case Qt::Key_Left: emulator->ReleaseButton(ButtonType::LEFT);
        break;
    case Qt::Key_Right: emulator->ReleaseButton(ButtonType::RIGHT);
        break;
    default: QMainWindow::keyReleaseEvent(event);
        break;
    }
}

void QtWindowSystem::closeEvent(QCloseEvent* event)
{
    shutdownEmulator();
    event->accept();
}
