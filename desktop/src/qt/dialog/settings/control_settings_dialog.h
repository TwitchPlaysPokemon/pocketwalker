#pragma once
#include <QDialog>
#include "desktop/src/qt/widget/key_bind_button.h"

class ControlSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ControlSettingsDialog(QWidget* parent = nullptr);

private slots:
    void apply();

private:
    KeyBindButton* left_button;
    KeyBindButton* right_button;
    KeyBindButton* center_button;
    KeyBindButton* step_button;
    KeyBindButton* speedup_button;
};