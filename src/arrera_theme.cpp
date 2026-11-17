#include "arrera_qt/arrera_theme.h"

#include <QFileInfo>

namespace {
QColor withAlpha(QColor col, int alpha)
{
    col.setAlpha(qBound(0, alpha, 255));
    return col;
}
} // namespace

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

ArreraThemeTokens Arrera_Theme::tokensForPalette(const QPalette &palette)
{
    ArreraThemeTokens tokens;
    const QColor windowCol = palette.color(QPalette::Window);
    const QColor windowText = palette.color(QPalette::WindowText);
    const bool isDark = windowCol.lightness() < 128;

    tokens.surfaceContainer = palette.color(QPalette::AlternateBase);
    tokens.surfaceElevated = palette.color(QPalette::Base);
    tokens.outline = withAlpha(windowText, isDark ? 64 : 50);
    tokens.outlineVariant = withAlpha(windowText, isDark ? 36 : 24);
    tokens.success = isDark ? QColor(QStringLiteral("#4ADE80")) : QColor(QStringLiteral("#16A34A"));
    tokens.warning = isDark ? QColor(QStringLiteral("#FBBF24")) : QColor(QStringLiteral("#D97706"));
    tokens.danger = isDark ? QColor(QStringLiteral("#E5484D")) : QColor(QStringLiteral("#D92D20"));
    tokens.onSemantic = QColor(Qt::white);

    return tokens;
}

ArreraThemeTokens Arrera_Theme::currentTokens()
{
    if (s_instance) {
        return s_instance->tokens();
    }
    return tokensForPalette(qApp ? qApp->palette() : QPalette());
}

const ArreraThemeTokens &Arrera_Theme::tokens() const
{
    return m_activeTokens;
}

const ArreraThemeTokens &Arrera_Theme::lightTokens() const
{
    return m_lightTokens;
}

const ArreraThemeTokens &Arrera_Theme::darkTokens() const
{
    return m_darkTokens;
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

    const QByteArray fileData = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(fileData, &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return false; // Fichier JSON invalide
    }

    setupDefaultPalettes();

    const QJsonObject rootObj = jsonDoc.object();

    // Parse la section "light"
    if (rootObj.contains(QStringLiteral("light")) && rootObj[QStringLiteral("light")].isObject()) {
        const QJsonObject lightObj = rootObj[QStringLiteral("light")].toObject();
        parsePaletteFromJson(lightObj, m_lightPalette);
        m_lightTokens = tokensForPalette(m_lightPalette);
        if (rootObj.contains(QStringLiteral("tokens")) && rootObj[QStringLiteral("tokens")].isObject()) {
            parseTokensFromJson(rootObj[QStringLiteral("tokens")].toObject(), m_lightTokens);
        }
        if (lightObj.contains(QStringLiteral("tokens")) && lightObj[QStringLiteral("tokens")].isObject()) {
            parseTokensFromJson(lightObj[QStringLiteral("tokens")].toObject(), m_lightTokens);
        }
    }

    // Parse la section "dark"
    if (rootObj.contains(QStringLiteral("dark")) && rootObj[QStringLiteral("dark")].isObject()) {
        const QJsonObject darkObj = rootObj[QStringLiteral("dark")].toObject();
        parsePaletteFromJson(darkObj, m_darkPalette);
        m_darkTokens = tokensForPalette(m_darkPalette);
        if (rootObj.contains(QStringLiteral("tokens")) && rootObj[QStringLiteral("tokens")].isObject()) {
            parseTokensFromJson(rootObj[QStringLiteral("tokens")].toObject(), m_darkTokens);
        }
        if (darkObj.contains(QStringLiteral("tokens")) && darkObj[QStringLiteral("tokens")].isObject()) {
            parseTokensFromJson(darkObj[QStringLiteral("tokens")].toObject(), m_darkTokens);
        }
    }

    m_currentThemeName = filePath;

    // Réappliquer le mode actuel (Auto / Light / Dark) avec les nouvelles couleurs
    applyTheme(activeColorScheme());
    return true;
}

void Arrera_Theme::parsePaletteFromJson(const QJsonObject &jsonObj, QPalette &palette)
{
    static const QMap<QString, QPalette::ColorRole> roleMap = {
        {QStringLiteral("Window"), QPalette::Window},
        {QStringLiteral("WindowText"), QPalette::WindowText},
        {QStringLiteral("Base"), QPalette::Base},
        {QStringLiteral("AlternateBase"), QPalette::AlternateBase},
        {QStringLiteral("ToolTipBase"), QPalette::ToolTipBase},
        {QStringLiteral("ToolTipText"), QPalette::ToolTipText},
        {QStringLiteral("Text"), QPalette::Text},
        {QStringLiteral("Button"), QPalette::Button},
        {QStringLiteral("ButtonText"), QPalette::ButtonText},
        {QStringLiteral("Highlight"), QPalette::Highlight},
        {QStringLiteral("HighlightedText"), QPalette::HighlightedText},
        {QStringLiteral("PlaceholderText"), QPalette::PlaceholderText},
        {QStringLiteral("Midlight"), QPalette::Midlight},
        {QStringLiteral("Mid"), QPalette::Mid}
    };

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        const QString key = it.key();
        if (roleMap.contains(key)) {
            const QColor color(it.value().toString());
            if (color.isValid()) {
                palette.setColor(roleMap[key], color);
            }
        }
    }
}

void Arrera_Theme::parseTokensFromJson(const QJsonObject &jsonObj, ArreraThemeTokens &tokens)
{
    auto readColor = [&jsonObj](const QString &key, QColor &target) {
        if (jsonObj.contains(key) && jsonObj[key].isString()) {
            const QColor c(jsonObj[key].toString());
            if (c.isValid()) {
                target = c;
            }
        }
    };

    auto readInt = [&jsonObj](const QString &key, int &target, int minVal) {
        if (jsonObj.contains(key) && jsonObj[key].isDouble()) {
            target = qMax(minVal, jsonObj[key].toInt());
        }
    };

    readColor(QStringLiteral("surfaceContainer"), tokens.surfaceContainer);
    readColor(QStringLiteral("surfaceElevated"), tokens.surfaceElevated);
    readColor(QStringLiteral("outline"), tokens.outline);
    readColor(QStringLiteral("outlineVariant"), tokens.outlineVariant);
    readColor(QStringLiteral("success"), tokens.success);
    readColor(QStringLiteral("warning"), tokens.warning);
    readColor(QStringLiteral("danger"), tokens.danger);
    readColor(QStringLiteral("onSemantic"), tokens.onSemantic);

    readInt(QStringLiteral("radiusSmall"), tokens.radiusSmall, 0);
    readInt(QStringLiteral("radiusMedium"), tokens.radiusMedium, 0);
    readInt(QStringLiteral("radiusLarge"), tokens.radiusLarge, 0);
    readInt(QStringLiteral("animationDuration"), tokens.animationDuration, 0);
}

void Arrera_Theme::applyTheme(Qt::ColorScheme scheme)
{
    const bool isDark = (scheme == Qt::ColorScheme::Dark);
    const QPalette targetPalette = isDark ? m_darkPalette : m_lightPalette;
    m_activeTokens = isDark ? m_darkTokens : m_lightTokens;

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
    m_lightPalette.setColor(QPalette::Window,          QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::WindowText,      QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Base,            QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::AlternateBase,   QColor("#F6F7FB"));
    m_lightPalette.setColor(QPalette::ToolTipBase,     QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::ToolTipText,     QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Text,            QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Button,          QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::ButtonText,      QColor("#0E1726"));
    m_lightPalette.setColor(QPalette::Highlight,       QColor("#4F46E5"));
    m_lightPalette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    m_lightPalette.setColor(QPalette::PlaceholderText, QColor(14, 23, 38, 140));
    m_lightPalette.setColor(QPalette::Midlight,        QColor("#EDF1F7"));
    m_lightPalette.setColor(QPalette::Mid,             QColor("#E3E8F0"));

    m_darkTokens = tokensForPalette(m_darkPalette);
    m_lightTokens = tokensForPalette(m_lightPalette);
}
