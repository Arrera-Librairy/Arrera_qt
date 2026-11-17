#include "test_ui.h"
#include "ui_test_ui.h"
#include "arrera_qt/acheckbox.h"
#include "arrera_qt/aframe.h"
#include "arrera_qt/alabel.h"
#include "arrera_qt/alineedit.h"
#include "arrera_qt/anavbutton.h"
#include "arrera_qt/aprogressbar.h"
#include "arrera_qt/apushbutton.h"
#include "arrera_qt/aradiobutton.h"
#include "arrera_qt/arrera_theme.h"
#include "arrera_qt/ascrollarea.h"
#include "arrera_qt/asegmentedbutton.h"
#include "arrera_qt/asettingrow.h"
#include "arrera_qt/aslider.h"
#include "arrera_qt/aswitch.h"
#include "arrera_qt/atoast.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QStyle>
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
    setupNavigationAndFeedbackShowcase();
    setupComfortShowcase();
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

void test_ui::setupNavigationAndFeedbackShowcase()
{
    Arrera_Theme *themeManager = Arrera_Theme::instance();

    // 1. ASegmentedButton relié dynamiquement au mode Auto / Clair / Sombre
    auto *feedbackRow = new QHBoxLayout();
    feedbackRow->setSpacing(14);

    auto *segmented = new ASegmentedButton(ui->frame);
    segmented->addSegment(tr("Auto"), QVariant::fromValue(Arrera_Theme::ThemeMode::Auto));
    segmented->addSegment(tr("Clair"), QVariant::fromValue(Arrera_Theme::ThemeMode::Light));
    segmented->addSegment(tr("Sombre"), QVariant::fromValue(Arrera_Theme::ThemeMode::Dark));
    segmented->setCurrentData(QVariant::fromValue(themeManager->themeMode()));

    connect(segmented, &ASegmentedButton::currentIndexChanged, this, [segmented, themeManager]() {
        themeManager->setThemeMode(segmented->currentData().value<Arrera_Theme::ThemeMode>());
    });
    connect(themeManager, &Arrera_Theme::themeModeChanged, this, [segmented](Arrera_Theme::ThemeMode mode) {
        const QSignalBlocker blocker(segmented);
        segmented->setCurrentData(QVariant::fromValue(mode));
    });

    // 2. ASlider + AProgressBar déterminée et indéterminée
    auto *slider = new ASlider(Qt::Horizontal, ui->frame);
    slider->setRange(0, 100);
    slider->setValue(45);

    auto *progressLayout = new QVBoxLayout();
    progressLayout->setSpacing(8);

    auto *progressDeterminate = new AProgressBar(ui->frame);
    progressDeterminate->setRange(0, 100);
    progressDeterminate->setValue(45);
    progressDeterminate->setTextVisible(true);

    auto *progressIndeterminate = new AProgressBar(ui->frame);
    progressIndeterminate->setRange(0, 0);

    connect(slider, &ASlider::valueChanged, progressDeterminate, &AProgressBar::setValue);

    progressLayout->addWidget(progressDeterminate);
    progressLayout->addWidget(progressIndeterminate);

    // 3. Aperçu AScrollArea / AScrollBar
    auto *scrollArea = new AScrollArea(ui->frame);
    scrollArea->setFixedHeight(74);
    scrollArea->setMinimumWidth(160);
    auto *scrollContent = new QWidget(scrollArea);
    auto *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(6, 4, 14, 4);
    scrollLayout->setSpacing(4);
    for (int i = 1; i <= 8; ++i) {
        scrollLayout->addWidget(new QLabel(tr("Élément défilant %1").arg(i), scrollContent));
    }
    scrollArea->setWidget(scrollContent);

    feedbackRow->addWidget(segmented);
    feedbackRow->addWidget(slider, 1);
    feedbackRow->addLayout(progressLayout, 1);
    feedbackRow->addWidget(scrollArea);

    ui->verticalLayout_2->insertLayout(5, feedbackRow);

    // 4. ANavButton dans AApplicationBar (frame_2)
    if (auto *barLayout = qobject_cast<QVBoxLayout *>(ui->frame_2->layout())) {
        auto *navHome = new ANavButton(style()->standardIcon(QStyle::SP_DirHomeIcon), tr("Accueil"), ui->frame_2);
        navHome->setChecked(true);
        auto *navFiles = new ANavButton(style()->standardIcon(QStyle::SP_DirIcon), tr("Projets"), ui->frame_2);
        auto *navPrefs = new ANavButton(style()->standardIcon(QStyle::SP_FileDialogDetailedView), tr("Paramètres"), ui->frame_2);

        barLayout->insertWidget(0, navHome);
        barLayout->insertWidget(1, navFiles);
        barLayout->insertWidget(2, navPrefs);
    }
}

void test_ui::setupComfortShowcase()
{
    // ALabel hiérarchie + ASettingRow + déclenchement AToast
    auto *titleLabel = new ALabel(tr("Arrera Qt — Galerie des composants"), ALabel::Typography::Title, ui->frame);
    auto *captionLabel = new ALabel(tr("Style hybride GTK4 (libadwaita) & Material 3 Expressive"), ALabel::Typography::Caption, ui->frame);

    ui->verticalLayout_2->insertWidget(0, titleLabel);
    ui->verticalLayout_2->insertWidget(1, captionLabel);

    auto *settingsCard = new AFrame(ui->frame);
    settingsCard->setCardStyle(AFrame::CardStyle::Elevated);
    auto *settingsLayout = new QVBoxLayout(settingsCard);
    settingsLayout->setContentsMargins(0, 4, 0, 4);
    settingsLayout->setSpacing(0);

    auto *rowSwitch = new ASettingRow(
        tr("Enregistrement automatique"),
        tr("Sauvegarde le thème actif dans QSettings"),
        settingsCard);
    rowSwitch->setIcon(style()->standardIcon(QStyle::SP_DialogSaveButton));
    rowSwitch->setActivatable(true);
    rowSwitch->setShowDivider(true);
    auto *autoSaveSwitch = new ASwitch(rowSwitch);
    autoSaveSwitch->setChecked(Arrera_Theme::instance()->isPersistenceEnabled());
    rowSwitch->setWidget(autoSaveSwitch);
    connect(autoSaveSwitch, &ASwitch::toggled, this, [this](bool enabled) {
        Arrera_Theme::instance()->setPersistenceEnabled(enabled);
        AToast::showToast(
            this,
            enabled ? tr("Persistance du thème activée") : tr("Persistance désactivée"),
            enabled ? AToast::Type::Success : AToast::Type::Info);
    });

    auto *rowToast = new ASettingRow(
        tr("Tester la notification flottante"),
        tr("Affiche un AToast en bas de la fenêtre"),
        settingsCard);
    rowToast->setIcon(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    auto *toastBtn = new APushButton(tr("Afficher Toast"), rowToast);
    toastBtn->setVariant(APushButton::Variant::Secondary);
    rowToast->setWidget(toastBtn);
    connect(toastBtn, &APushButton::clicked, this, [this]() {
        auto *toast = AToast::showToast(this, tr("Thème appliqué avec succès"), AToast::Type::Success, 4000);
        toast->setAction(tr("OK"));
    });

    settingsLayout->addWidget(rowSwitch);
    settingsLayout->addWidget(rowToast);

    ui->verticalLayout_2->insertWidget(ui->verticalLayout_2->count() - 1, settingsCard);

    ui->pushButton->setText(tr("Tester Toast Info"));
    connect(ui->pushButton, &APushButton::clicked, this, [this]() {
        AToast::showToast(this, tr("Action déclenchée depuis APushButton"), AToast::Type::Info);
    });
}
