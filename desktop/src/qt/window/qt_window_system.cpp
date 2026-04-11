#include "qt_window_system.h"
#include "widgets/display_widget.h"
#include <fstream>
#include <QMenuBar>
#include <QTimer>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>

QtWindowSystem::QtWindowSystem(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("PocketWalker");
    menuBar()->setNativeMenuBar(true);

    auto* file_menu = menuBar()->addMenu("File");
    connect(file_menu->addAction("Open ROM"), &QAction::triggered, this, &QtWindowSystem::openROM);
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

void QtWindowSystem::launchEmulator(const std::string& rom_path)
{
    shutdownEmulator();

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
        EepromBuffer buf = emulator->GetEepromBuffer();
        std::ofstream f(save_path, std::ios::binary);
        f.write(reinterpret_cast<const char*>(buf.data()), buf.size());
    }

    emulator.reset();
    display->setEmulator(nullptr);

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
