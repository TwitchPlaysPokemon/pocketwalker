#pragma once
#include <QDialog>
#include <QSlider>
#include <QLabel>

class AudioSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AudioSettingsDialog(QWidget* parent = nullptr);

private slots:
    void apply();

private:
    QSlider* volume_slider;
    QLabel* volume_label;
};
