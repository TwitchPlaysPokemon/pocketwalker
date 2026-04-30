#include "control_settings_dialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>

#include "desktop/src/qt/settings/app_settings.h"
#include "desktop/src/qt/settings/types/control_settings.h"

ControlSettingsDialog::ControlSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("Control Settings");
    setMinimumWidth(400);

    auto& controls = AppSettings::instance.controls;

    auto make_button = [this](int key)
    {
        auto* btn = new KeyBindButton(this);
        btn->setKey(key);
        return btn;
    };

    left_button = make_button(controls.key_left);
    right_button = make_button(controls.key_right);
    center_button = make_button(controls.key_center);
    step_button = make_button(controls.key_step);
    speedup_button = make_button(controls.key_speedup);

    auto* controls_form = new QFormLayout();
    controls_form->addRow("Left:", left_button);
    controls_form->addRow("Right:", right_button);
    controls_form->addRow("Center:", center_button);
    controls_form->addRow("Take Step:", step_button);
    controls_form->addRow("Speed Up:", speedup_button);

    auto* controls_group = new QGroupBox("Keys", this);
    controls_group->setLayout(controls_form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(controls_group);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, &ControlSettingsDialog::apply);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void ControlSettingsDialog::apply()
{
    auto& controls = AppSettings::instance.controls;

    controls.key_left = left_button->key();
    controls.key_right = right_button->key();
    controls.key_center = center_button->key();
    controls.key_step = step_button->key();
    controls.key_speedup = speedup_button->key();

    accept();
}
