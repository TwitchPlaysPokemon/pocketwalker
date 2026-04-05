#include "qt_window_system.h"
#include "widgets/display_widget.h"
#include <QMenuBar>
#include <QTimer>
#include <QApplication>

QtWindowSystem::QtWindowSystem(PocketWalker& emulator, QWidget* parent)
    : QMainWindow(parent), emulator(emulator)
{
    setWindowTitle("PocketWalker");
    menuBar()->setNativeMenuBar(true);

    auto* file_menu = menuBar()->addMenu("File");
    file_menu->addAction("Open ROM");
    file_menu->addAction("Open Save");
    file_menu->addSeparator();
    connect(file_menu->addAction("Exit"), &QAction::triggered, qApp, &QApplication::quit);

    auto* settings_menu = menuBar()->addMenu("Settings");
    settings_menu->addAction("Emulation");
    settings_menu->addAction("Control");
    settings_menu->addAction("IR");

    auto* display = new DisplayWidget(emulator, this);
    setCentralWidget(display);
    adjustSize();

    auto* render_timer = new QTimer(this);
    connect(render_timer, &QTimer::timeout, display, QOverload<>::of(&QWidget::update));
    render_timer->start(16);
}

void QtWindowSystem::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Down:
        emulator.PressButton(ButtonType::CENTER);
        break;
    case Qt::Key_Left:
        emulator.PressButton(ButtonType::LEFT);
        break;
    case Qt::Key_Right:
        emulator.PressButton(ButtonType::RIGHT);
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void QtWindowSystem::keyReleaseEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Down:
        emulator.ReleaseButton(ButtonType::CENTER);
        break;
    case Qt::Key_Left:
        emulator.ReleaseButton(ButtonType::LEFT);
        break;
    case Qt::Key_Right:
        emulator.ReleaseButton(ButtonType::RIGHT);
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
        break;
    }
}

void QtWindowSystem::closeEvent(QCloseEvent* event)
{
    emulator.Stop();

    event->accept();
}
