#include <QApplication>
#include "test_ui.h"
#include "arrera_qt/arrera_theme.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Initialisation optionnelle du thème si besoin
    Arrera_Theme themeManager;
    themeManager.loadThemeFromJson("theme/theme_default.json");

    test_ui window;
    window.show();


    return a.exec();
}
