#pragma once
#include <QMainWindow>
#include <QKeyEvent>
#include "core/pokewalker/pocketwalker.h"

class QtWindowSystem : public QMainWindow
{
    Q_OBJECT
public:
    explicit QtWindowSystem(PocketWalker& emulator, QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    PocketWalker& emulator;
};