#include "arrera_qt/arrera_theme.h"

#include <QFileInfo>

Arrera_Theme *Arrera_Theme::s_instance = nullptr;

Arrera_Theme::Arrera_Theme(QObject *parent)
    : QObject(parent)
    , m_themeMode(ThemeMode::Auto)
    , m_currentThemeName(QStringLiteral("default"))
    , m_persistenceEnabled(false)
    , m_settingsGroup(QStringLiteral("ArreraTheme"))
{
    if (!s_instance) {
        s_instance = this;
    }

    QApplication::setStyle(QStringLiteral("fusion"));

    setupDefaultPalettes();

    connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
            this, &Arrera_Theme::onSystemColorSchemeChanged);

    applyTheme(activeColorScheme());
}

Arrera_Theme::~Arrera_Theme()
{
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

Arrera_Theme *Arrera_Theme::instance()
{
    if (!s_instance) {
        s_instance = new Arrera_Theme(qApp);
    }
    return s_instance;
}

QStringList Arrera_Theme::availableThemes()
{
    QDir themeDir(QStringLiteral(":/arrera_qt/theme"));
    const QStringList files = themeDir.entryList({QStringLiteral("theme_*.json")}, QDir::Files, QDir::Name);

    QStringList themes;
    themes.reserve(files.size());
    for (const QString &file : files) {
        QString name = file.mid(6); // retire "theme_"
        name.chop(5);               // retire ".json"
        if (!name.isEmpty()) {
            themes.append(name);
        }
    }

    if (themes.removeOne(QStringLiteral("default"))) {
        themes.prepend(QStringLiteral("default"));
    }

    return themes;
}

QString Arrera_Theme::resolveThemePath(const QString &themeNameOrPath, QString *normalizedName)
{
    if (themeNameOrPath.endsWith(QStringLiteral(".json"), Qt::CaseInsensitive)
        || QFileInfo::exists(themeNameOrPath)) {
        if (normalizedName) {
            *normalizedName = themeNameOrPath;
        }
        return themeNameOrPath;
    }

    if (normalizedName) {
        *normalizedName = themeNameOrPath;
    }
    return QStringLiteral(":/arrera_qt/theme/theme_%1.json").arg(themeNameOrPath);
}

bool Arrera_Theme::setTheme(const QString &themeNameOrPath)
{
    QString normalizedName;
    const QString path = resolveThemePath(themeNameOrPath, &normalizedName);

    if (!loadThemeFromJson(path)) {
        return false;
    }

    m_currentThemeName = normalizedName;
    saveSettings();
    emit themeChanged(m_currentThemeName);
    return true;
}

bool Arrera_Theme::loadThemeFromJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return false; // Fichier JSON invalide
    }

    setupDefaultPalettes();

    QJsonObject rootObj = jsonDoc.object();

    // Parse la section "light"
    if (rootObj.contains("light") && rootObj["light"].isObject()) {
        parsePaletteFromJson(rootObj["light"].toObject(), m_lightPalette);
    }

    // Parse la section "dark"
    if (rootObj.contains("dark") && rootObj["dark"].isObject()) {
        parsePaletteFromJson(rootObj["dark"].toObject(), m_darkPalette);
    }

    m_currentThemeName = filePath;

    // Réappliquer le mode actuel (Auto / Light / Dark) avec les nouvelles couleurs
    applyTheme(activeColorScheme());
    return true;
}

void Arrera_Theme::parsePaletteFromJson(const QJsonObject &jsonObj, QPalette &palette)
{
    // Dictionnaire pour lier les chaînes de caractères du JSON aux rôles Qt
    static const QMap<QString, QPalette::ColorRole> roleMap = {
        {"Window", QPalette::Window},
        {"WindowText", QPalette::WindowText},
        {"Base", QPalette::Base},
        {"AlternateBase", QPalette::AlternateBase},
        {"ToolTipBase", QPalette::ToolTipBase},
        {"ToolTipText", QPalette::ToolTipText},
        {"Text", QPalette::Text},
        {"Button", QPalette::Button},
        {"ButtonText", QPalette::ButtonText},
        {"Highlight", QPalette::Highlight},
        {"HighlightedText", QPalette::HighlightedText},
        {"PlaceholderText", QPalette::PlaceholderText},
        {"Midlight", QPalette::Midlight},
        {"Mid", QPalette::Mid}
    };

    // Parcourir toutes les clés de l'objet JSON (ex: "Window", "Text", etc.)
    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        QString key = it.key();
        if (roleMap.contains(key)) {
            // Si la clé existe dans notre dictionnaire, on applique la couleur
            QColor color(it.value().toString());
            if (color.isValid()) {
                palette.setColor(roleMap[key], color);
            }
        }
    }
}

void Arrera_Theme::applyTheme(Qt::ColorScheme scheme)
{
    QPalette targetPalette = (scheme == Qt::ColorScheme::Dark) ? m_darkPalette : m_lightPalette;

    qApp->setPalette(targetPalette);

    for (QWidget *topLevel : QApplication::topLevelWidgets()) {
        topLevel->setPalette(targetPalette);
    }
    for (QWidget *widget : QApplication::allWidgets()) {
        widget->update();
    }

    emit colorSchemeChanged(scheme);
}

void Arrera_Theme::setThemeMode(ThemeMode mode)
{
    if (m_themeMode == mode) {
        return;
    }

    m_themeMode = mode;
    applyTheme(activeColorScheme());
    saveSettings();
    emit themeModeChanged(m_themeMode);
}

Arrera_Theme::ThemeMode Arrera_Theme::themeMode() const
{
    return m_themeMode;
}

QString Arrera_Theme::currentTheme() const
{
    return m_currentThemeName;
}

Qt::ColorScheme Arrera_Theme::activeColorScheme() const
{
    switch (m_themeMode) {
    case ThemeMode::Light:
        return Qt::ColorScheme::Light;
    case ThemeMode::Dark:
        return Qt::ColorScheme::Dark;
    case ThemeMode::Auto:
    default:
        return qApp->styleHints()->colorScheme();
    }
}

void Arrera_Theme::setAutoSystemThemeTracking(bool enable)
{
    if (enable) {
        setThemeMode(ThemeMode::Auto);
    } else {
        const ThemeMode frozenMode = (activeColorScheme() == Qt::ColorScheme::Dark)
                                         ? ThemeMode::Dark
                                         : ThemeMode::Light;
        setThemeMode(frozenMode);
    }
}

void Arrera_Theme::onSystemColorSchemeChanged(Qt::ColorScheme scheme)
{
    if (m_themeMode == ThemeMode::Auto) {
        applyTheme(scheme);
    }
}

void Arrera_Theme::setPersistenceEnabled(bool enable, const QString &settingsGroup)
{
    m_persistenceEnabled = enable;
    if (!settingsGroup.isEmpty()) {
        m_settingsGroup = settingsGroup;
    }

    if (m_persistenceEnabled) {
        restoreSavedTheme();
    }
}

bool Arrera_Theme::isPersistenceEnabled() const
{
    return m_persistenceEnabled;
}

void Arrera_Theme::restoreSavedTheme()
{
    QSettings settings;
    settings.beginGroup(m_settingsGroup);

    if (settings.contains(QStringLiteral("mode"))) {
        const int savedMode = settings.value(QStringLiteral("mode"), static_cast<int>(ThemeMode::Auto)).toInt();
        if (savedMode >= static_cast<int>(ThemeMode::Auto) && savedMode <= static_cast<int>(ThemeMode::Dark)) {
            m_themeMode = static_cast<ThemeMode>(savedMode);
        }
    }

    const QString savedTheme = settings.value(QStringLiteral("theme")).toString();
    settings.endGroup();

    if (!savedTheme.isEmpty() && setTheme(savedTheme)) {
        emit themeModeChanged(m_themeMode);
    } else {
        applyTheme(activeColorScheme());
    }
}

void Arrera_Theme::saveSettings() const
{
    if (!m_persistenceEnabled) {
        return;
    }

    QSettings settings;
    settings.beginGroup(m_settingsGroup);
    settings.setValue(QStringLiteral("theme"), m_currentThemeName);
    settings.setValue(QStringLiteral("mode"), static_cast<int>(m_themeMode));
    settings.endGroup();
}

void Arrera_Theme::setupDefaultPalettes()
{
    m_darkPalette = QPalette();
    m_lightPalette = QPalette();

    // === Palette Sombre (Fallback) ===
    m_darkPalette.setColor(QPalette::Window, QColor("#0E1116"));
    m_darkPalette.setColor(QPalette::WindowText, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Base, QColor("#1E1E1E"));
    m_darkPalette.setColor(QPalette::AlternateBase, QColor("#12161F"));
    m_darkPalette.setColor(QPalette::ToolTipBase, QColor("#11151C"));
    m_darkPalette.setColor(QPalette::ToolTipText, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Text, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Button, QColor("#11151C"));
    m_darkPalette.setColor(QPalette::ButtonText, QColor("#E8EDF2"));
    m_darkPalette.setColor(QPalette::Highlight, QColor("#6D5EFC"));
    m_darkPalette.setColor(QPalette::HighlightedText, QColor("#0B0F14"));

    // === Palette Claire (Fallback) ===
    m_lightPalette.setColor(QPalette::Window,         QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::WindowText,     QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Base,           QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::AlternateBase,  QColor("#F6F7FB"));
    m_lightPalette.setColor(QPalette::ToolTipBase,    QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::ToolTipText,    QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Text,           QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Button,         QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::ButtonText,     QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Highlight,      QColor("#4F46E5"));
    m_lightPalette.setColor(QPalette::HighlightedText,QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::PlaceholderText,QColor(14,23,38,140));
    m_lightPalette.setColor(QPalette::Midlight,       QColor("#EDF1F7"));
    m_lightPalette.setColor(QPalette::Mid,            QColor("#E3E8F0"));
}
