#include "test_ui.h"
#include "ui_test_ui.h"
#include "arrera_qt/acheckbox.h"
#include "arrera_qt/aframe.h"
#include "arrera_qt/alineedit.h"
#include "arrera_qt/apushbutton.h"
#include "arrera_qt/aradiobutton.h"
#include "arrera_qt/arrera_theme.h"
#include "arrera_qt/aswitch.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QVBoxLayout>

test_ui::test_ui(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::test_ui)
{
    ui->setupUi(this);
    ui->lineEdit->setPlaceholderText(tr("Rechercher dans Arrera Qt..."));
    setupThemeControls();
    setupButtonShowcase();
    setupFrameShowcase();
    setupControlsShowcase();
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

void test_ui::setupButtonShowcase()
{
    ui->pushButton->setText(tr("Primary"));

    auto *rowLayout = new QHBoxLayout();
    rowLayout->setSpacing(10);

    auto *secondaryBtn = new APushButton(tr("Secondary"), ui->frame);
    secondaryBtn->setVariant(APushButton::Variant::Secondary);

    auto *outlinedBtn = new APushButton(tr("Outlined"), ui->frame);
    outlinedBtn->setVariant(APushButton::Variant::Outlined);

    auto *flatBtn = new APushButton(tr("Flat"), ui->frame);
    flatBtn->setVariant(APushButton::Variant::Flat);

    auto *toggleBtn = new APushButton(tr("Toggle"), ui->frame);
    toggleBtn->setVariant(APushButton::Variant::Outlined);
    toggleBtn->setCheckable(true);
    toggleBtn->setChecked(true);

    auto *destructiveBtn = new APushButton(tr("Destructive"), ui->frame);
    destructiveBtn->setVariant(APushButton::Variant::Destructive);

    auto *disabledBtn = new APushButton(tr("Disabled"), ui->frame);
    disabledBtn->setEnabled(false);

    rowLayout->addWidget(secondaryBtn);
    rowLayout->addWidget(outlinedBtn);
    rowLayout->addWidget(flatBtn);
    rowLayout->addWidget(toggleBtn);
    rowLayout->addWidget(destructiveBtn);
    rowLayout->addWidget(disabledBtn);
    rowLayout->addStretch();

    ui->verticalLayout_2->insertLayout(1, rowLayout);
}

void test_ui::setupFrameShowcase()
{
    auto *cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(12);

    const struct {
        QString title;
        AFrame::CardStyle style;
    } specs[] = {
        {tr("AFrame Filled"), AFrame::CardStyle::Filled},
        {tr("AFrame Elevated"), AFrame::CardStyle::Elevated},
        {tr("AFrame Outlined"), AFrame::CardStyle::Outlined}
    };

    for (const auto &spec : specs) {
        auto *card = new AFrame(ui->frame);
        card->setCardStyle(spec.style);
        card->setMinimumHeight(72);

        auto *cardLayout = new QVBoxLayout(card);
        cardLayout->setContentsMargins(16, 12, 16, 12);

        auto *label = new QLabel(spec.title, card);
        QFont f = label->font();
        f.setWeight(QFont::Medium);
        label->setFont(f);
        cardLayout->addWidget(label);

        cardsLayout->addWidget(card);
    }

    ui->verticalLayout_2->insertLayout(2, cardsLayout);
}

void test_ui::setupControlsShowcase()
{
    auto *inputsLayout = new QHBoxLayout();
    inputsLayout->setSpacing(12);

    auto *nameEdit = new ALineEdit(tr("Nom d'utilisateur"), ui->frame);
    nameEdit->setClearButtonEnabled(true);

    auto *passwordEdit = new ALineEdit(tr("Mot de passe"), ui->frame);
    passwordEdit->setEchoMode(QLineEdit::Password);

    inputsLayout->addWidget(nameEdit);
    inputsLayout->addWidget(passwordEdit);

    auto *togglesLayout = new QHBoxLayout();
    togglesLayout->setSpacing(16);

    auto *switchOff = new ASwitch(tr("Notifications"), ui->frame);
    auto *switchOn = new ASwitch(tr("Mode compact"), ui->frame);
    switchOn->setChecked(true);

    auto *checkBox1 = new ACheckBox(tr("Option A"), ui->frame);
    checkBox1->setChecked(true);
    auto *checkBox2 = new ACheckBox(tr("Option B"), ui->frame);

    auto *radio1 = new ARadioButton(tr("Choix 1"), ui->frame);
    radio1->setChecked(true);
    auto *radio2 = new ARadioButton(tr("Choix 2"), ui->frame);

    togglesLayout->addWidget(switchOff);
    togglesLayout->addWidget(switchOn);
    togglesLayout->addWidget(checkBox1);
    togglesLayout->addWidget(checkBox2);
    togglesLayout->addWidget(radio1);
    togglesLayout->addWidget(radio2);
    togglesLayout->addStretch();

    ui->verticalLayout_2->insertLayout(3, inputsLayout);
    ui->verticalLayout_2->insertLayout(4, togglesLayout);
}
