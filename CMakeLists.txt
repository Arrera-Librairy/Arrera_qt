cmake_minimum_required(VERSION 3.16)
project(arrera_qt VERSION 1.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" ARRERA_QT_IS_TOP_LEVEL)
option(ARRERA_QT_BUILD_EXAMPLES "Build the arrera_qt demo application" ${ARRERA_QT_IS_TOP_LEVEL})

qt_add_library(arrera_qt STATIC
    include/arrera_qt/aapplicationbar.h
    include/arrera_qt/aframe.h
    include/arrera_qt/apushbutton.h
    include/arrera_qt/arrera_theme.h
    include/arrera_qt/asearchbar.h
    src/aapplicationbar.cpp
    src/aframe.cpp
    src/apushbutton.cpp
    src/arrera_theme.cpp
    src/asearchbar.cpp
)
add_library(Arrera::Qt ALIAS arrera_qt)

target_compile_features(arrera_qt PUBLIC cxx_std_17)

target_include_directories(arrera_qt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_definitions(arrera_qt PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(arrera_qt PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

qt_add_resources(arrera_qt "arrera_qt_themes"
    PREFIX "/arrera_qt"
    FILES
        theme/theme_blanc.json
        theme/theme_blanc_gris.json
        theme/theme_bleu.json
        theme/theme_bleu_blanc.json
        theme/theme_bleu_violet.json
        theme/theme_default.json
        theme/theme_gris.json
        theme/theme_jaune.json
        theme/theme_orange.json
        theme/theme_rose.json
        theme/theme_rouge.json
)

if(ARRERA_QT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
