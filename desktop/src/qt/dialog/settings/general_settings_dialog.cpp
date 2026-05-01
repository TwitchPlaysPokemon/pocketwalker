#include "general_settings_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QFileDialog>

#include "desktop/src/qt/settings/app_settings.h"
#include "desktop/src/qt/settings/types/general_settings.h"

GeneralSettingsDialog::GeneralSettingsDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle("General Settings");
    setMinimumWidth(400);

    auto& general = AppSettings::instance.general;

    theme_combo = new QComboBox(this);
    theme_combo->addItem("System", static_cast<int>(GeneralSettings::AppTheme::System));
    theme_combo->addItem("Light", static_cast<int>(GeneralSettings::AppTheme::Light));
    theme_combo->addItem("Dark", static_cast<int>(GeneralSettings::AppTheme::Dark));
    theme_combo->setCurrentIndex(static_cast<int>(general.theme));

    auto* appearance_form = new QFormLayout();
    appearance_form->addRow("Theme:", theme_combo);

    auto* appearance_group = new QGroupBox("Appearance", this);
    appearance_group->setLayout(appearance_form);

    boot_on_launch_check = new QCheckBox("Boot on Launch", this);
    boot_on_launch_check->setChecked(general.boot_on_launch);

    default_rom_edit = new QLineEdit(QString::fromStdString(general.default_rom), this);
    default_rom_edit->setPlaceholderText("No ROM selected");
    default_rom_edit->setEnabled(general.boot_on_launch);

    browse_rom_btn = new QPushButton("Browse...", this);
    browse_rom_btn->setEnabled(general.boot_on_launch);

    auto* rom_row = new QHBoxLayout();
    rom_row->addWidget(default_rom_edit);
    rom_row->addWidget(browse_rom_btn);

    auto* application_form = new QFormLayout();
    application_form->addRow(boot_on_launch_check);
    application_form->addRow("Default ROM:", rom_row);

    auto* application_group = new QGroupBox("Application", this);
    application_group->setLayout(application_form);

    auto* tweaks_form = new QFormLayout();
    auto* tweaks_group = new QGroupBox("Tweaks", this);
    tweaks_group->setLayout(tweaks_form);

    prevent_activity_timeout_check = new QCheckBox("Prevent Activity Timeout", this);
    prevent_activity_timeout_check->setChecked(general.prevent_activity_timeout);
    tweaks_form->addRow(prevent_activity_timeout_check);

    force_walking_state_check = new QCheckBox("Force Walking State When Steps Remaining", this);
    force_walking_state_check->setChecked(general.force_walking_state);
    tweaks_form->addRow(force_walking_state_check);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(appearance_group);
    layout->addWidget(application_group);
    layout->addWidget(tweaks_group);
    layout->addWidget(buttons);

    connect(boot_on_launch_check, &QCheckBox::toggled, this, &GeneralSettingsDialog::onBootOnLaunchToggled);
    connect(browse_rom_btn, &QPushButton::clicked, this, &GeneralSettingsDialog::onBrowseRom);
    connect(buttons, &QDialogButtonBox::accepted, this, &GeneralSettingsDialog::apply);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void GeneralSettingsDialog::onBootOnLaunchToggled(bool enabled)
{
    default_rom_edit->setEnabled(enabled);
    browse_rom_btn->setEnabled(enabled);
}

void GeneralSettingsDialog::onBrowseRom()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Select Default ROM", QString(), "ROM Files (*.bin *.rom);;All Files (*)");

    if (!path.isEmpty())
        default_rom_edit->setText(path);
}

void GeneralSettingsDialog::apply()
{
    auto& general = AppSettings::instance.general;

    const auto theme = static_cast<GeneralSettings::AppTheme>(theme_combo->currentData().toInt());
    const bool theme_changed = theme != general.theme;
    const bool prevent_activity_timeout_changed = prevent_activity_timeout_check->isChecked() != general.prevent_activity_timeout;
    const bool force_walking_state_changed = force_walking_state_check->isChecked() != general.force_walking_state;

    general.theme = theme;
    general.boot_on_launch = boot_on_launch_check->isChecked();
    general.prevent_activity_timeout = prevent_activity_timeout_check->isChecked();
    general.force_walking_state = force_walking_state_check->isChecked();
    general.default_rom = default_rom_edit->text().toStdString();

    if (theme_changed)
        emit themeChanged();

    if (prevent_activity_timeout_changed)
        emit preventActivityTimeoutChanged();

    if (force_walking_state_changed)
        emit forceWalkingStateChanged();

    accept();
}
