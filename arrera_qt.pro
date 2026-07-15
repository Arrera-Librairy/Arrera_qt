QT += core gui widgets

TARGET = arrera_qt_test
TEMPLATE = app

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/arrera_qt

SOURCES += \
    arrera_qt/roundedframe.cpp \
    main.cpp \
    arrera_qt/apushbutton.cpp \
    arrera_qt/asearchbar.cpp \
    arrera_qt/arrera_theme.cpp \
    test_ui.cpp

HEADERS += \
    arrera_qt/apushbutton.h \
    arrera_qt/asearchbar.h \
    arrera_qt/arrera_theme.h \
    arrera_qt/roundedframe.h \
    test_ui.h

# On s'assure que le dossier theme est facilement accessible
# (Optionnel: vous pourriez aussi utiliser un fichier .qrc pour inclure les thèmes directement dans l'exécutable)

FORMS += \
    test_ui.ui
