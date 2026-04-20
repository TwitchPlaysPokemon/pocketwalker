#include "audio_settings_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

#include "desktop/src/qt/settings/app_settings.h"
#include "desktop/src/qt/settings/types/audio_settings.h"

AudioSettingsDialog::AudioSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Audio Settings");
    setMinimumWidth(400);

    auto& audio = AppSettings::instance.audio;

    volume_slider = new QSlider(Qt::Horizontal, this);
    volume_slider->setRange(0, 100);
    volume_slider->setValue(static_cast<int>(audio.volume * 100));

    volume_label = new QLabel(QString::number(audio.volume, 'f', 2), this);
    volume_label->setMinimumWidth(35);

    auto* volume_row = new QHBoxLayout();
    volume_row->addWidget(volume_slider);
    volume_row->addWidget(volume_label);

    connect(volume_slider, &QSlider::valueChanged, this, [this](int val)
    {
        volume_label->setText(QString::number(val / 100.0f, 'f', 2));
    });

    auto* sound_form = new QFormLayout();
    sound_form->addRow("Volume:", volume_row);

    auto* sound_group = new QGroupBox("Sound", this);
    sound_group->setLayout(sound_form);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(sound_group);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &AudioSettingsDialog::apply);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AudioSettingsDialog::apply()
{
    auto& audio = AppSettings::instance.audio;

    audio.volume = volume_slider->value() / 100.0f;

    accept();
}
