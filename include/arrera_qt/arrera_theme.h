#ifndef ARRERA_THEME_H
#define ARRERA_THEME_H

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMap>
#include <QObject>
#include <QPalette>
#include <QSettings>
#include <QStringList>
#include <QStyleHints>
#include <QWidget>

class Arrera_Theme : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentTheme READ currentTheme NOTIFY themeChanged)
    Q_PROPERTY(ThemeMode themeMode READ themeMode WRITE setThemeMode NOTIFY themeModeChanged)

public:
    enum class ThemeMode {
        Auto,
        Light,
        Dark
    };
    Q_ENUM(ThemeMode)

    explicit Arrera_Theme(QObject *parent = nullptr);
    ~Arrera_Theme() override;

    static Arrera_Theme *instance();
    static QStringList availableThemes();

    bool loadThemeFromJson(const QString &filePath);
    void applyTheme(Qt::ColorScheme scheme);
    void setAutoSystemThemeTracking(bool enable);

    QString currentTheme() const;
    ThemeMode themeMode() const;
    Qt::ColorScheme activeColorScheme() const;

    void setPersistenceEnabled(bool enable, const QString &settingsGroup = QStringLiteral("ArreraTheme"));
    bool isPersistenceEnabled() const;
    void restoreSavedTheme();

public slots:
    bool setTheme(const QString &themeNameOrPath);
    void setThemeMode(ThemeMode mode);
    void onSystemColorSchemeChanged(Qt::ColorScheme scheme);

signals:
    void themeChanged(const QString &themeName);
    void themeModeChanged(ThemeMode mode);
    void colorSchemeChanged(Qt::ColorScheme scheme);

private:
    void setupDefaultPalettes();
    void parsePaletteFromJson(const QJsonObject &jsonObj, QPalette &palette);
    void saveSettings() const;
    static QString resolveThemePath(const QString &themeNameOrPath, QString *normalizedName = nullptr);

    static Arrera_Theme *s_instance;

    ThemeMode m_themeMode;
    QString m_currentThemeName;
    bool m_persistenceEnabled;
    QString m_settingsGroup;
    QPalette m_lightPalette;
    QPalette m_darkPalette;
};

#endif // ARRERA_THEME_H
