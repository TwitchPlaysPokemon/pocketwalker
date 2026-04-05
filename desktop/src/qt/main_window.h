#pragma once
#include <QMainWindow>
#include <QKeyEvent>
#include "core/pokewalker/pocketwalker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(PocketWalker& emulator, QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    PocketWalker& emulator;
};