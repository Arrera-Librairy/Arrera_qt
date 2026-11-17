#include <QApplication>
#include "test_ui.h"
#include "arrera_qt/arrera_theme.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Arrera Softwares"));
    QCoreApplication::setApplicationName(QStringLiteral("ArreraQtTest"));

    Arrera_Theme themeManager;
    themeManager.setTheme(QStringLiteral("default"));
    themeManager.setPersistenceEnabled(true);

    test_ui window;
    window.show();

    return a.exec();
}
