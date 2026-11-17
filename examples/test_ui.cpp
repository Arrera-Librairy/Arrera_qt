#include "test_ui.h"
#include "ui_test_ui.h"
#include "arrera_qt/arrera_theme.h"

#include <QSignalBlocker>

test_ui::test_ui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::test_ui)
{
    ui->setupUi(this);
    setupThemeControls();
}

test_ui::~test_ui()
{
    delete ui;
}

void test_ui::setupThemeControls()
{
    Arrera_Theme *themeManager = Arrera_Theme::instance();

    ui->themeComboBox->addItems(Arrera_Theme::availableThemes());
    const int currentThemeIdx = ui->themeComboBox->findText(themeManager->currentTheme());
    if (currentThemeIdx >= 0) {
        ui->themeComboBox->setCurrentIndex(currentThemeIdx);
    }

    ui->modeComboBox->addItem(tr("Auto (Système)"), QVariant::fromValue(Arrera_Theme::ThemeMode::Auto));
    ui->modeComboBox->addItem(tr("Clair"), QVariant::fromValue(Arrera_Theme::ThemeMode::Light));
    ui->modeComboBox->addItem(tr("Sombre"), QVariant::fromValue(Arrera_Theme::ThemeMode::Dark));

    const int currentModeIdx = ui->modeComboBox->findData(QVariant::fromValue(themeManager->themeMode()));
    if (currentModeIdx >= 0) {
        ui->modeComboBox->setCurrentIndex(currentModeIdx);
    }

    connect(ui->themeComboBox, &QComboBox::currentTextChanged,
            themeManager, &Arrera_Theme::setTheme);

    connect(ui->modeComboBox, &QComboBox::currentIndexChanged, this, [this, themeManager](int index) {
        if (index < 0) {
            return;
        }
        const auto mode = ui->modeComboBox->itemData(index).value<Arrera_Theme::ThemeMode>();
        themeManager->setThemeMode(mode);
    });

    connect(themeManager, &Arrera_Theme::themeChanged, this, [this](const QString &name) {
        const QSignalBlocker blocker(ui->themeComboBox);
        const int idx = ui->themeComboBox->findText(name);
        if (idx >= 0) {
            ui->themeComboBox->setCurrentIndex(idx);
        }
    });

    connect(themeManager, &Arrera_Theme::themeModeChanged, this, [this](Arrera_Theme::ThemeMode mode) {
        const QSignalBlocker blocker(ui->modeComboBox);
        const int idx = ui->modeComboBox->findData(QVariant::fromValue(mode));
        if (idx >= 0) {
            ui->modeComboBox->setCurrentIndex(idx);
        }
    });
}
