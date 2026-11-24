cmake_minimum_required(VERSION 3.16)
project(arrera_qt VERSION 1.0.1 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

string(COMPARE EQUAL "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}" ARRERA_QT_IS_TOP_LEVEL)
option(ARRERA_QT_BUILD_EXAMPLES "Build the arrera_qt demo application" ${ARRERA_QT_IS_TOP_LEVEL})
option(ARRERA_QT_BUILD_DESIGNER_PLUGIN "Build Qt Designer custom widget plugin" OFF)
option(ARRERA_QT_INSTALL "Enable install targets for arrera_qt" ${ARRERA_QT_IS_TOP_LEVEL})

qt_add_library(arrera_qt STATIC
    include/arrera_qt/aapplicationbar.h
    include/arrera_qt/acheckbox.h
    include/arrera_qt/acombobox.h
    include/arrera_qt/aframe.h
    include/arrera_qt/alabel.h
    include/arrera_qt/alineedit.h
    include/arrera_qt/anavbutton.h
    include/arrera_qt/aprogressbar.h
    include/arrera_qt/apushbutton.h
    include/arrera_qt/aradiobutton.h
    include/arrera_qt/arrera_theme.h
    include/arrera_qt/ascrollarea.h
    include/arrera_qt/ascrollbar.h
    include/arrera_qt/asearchbar.h
    include/arrera_qt/asegmentedbutton.h
    include/arrera_qt/asettingrow.h
    include/arrera_qt/aslider.h
    include/arrera_qt/aswitch.h
    include/arrera_qt/atoast.h
    src/aapplicationbar.cpp
    src/acheckbox.cpp
    src/acombobox.cpp
    src/aframe.cpp
    src/alabel.cpp
    src/alineedit.cpp
    src/anavbutton.cpp
    src/aprogressbar.cpp
    src/apushbutton.cpp
    src/aradiobutton.cpp
    src/arrera_theme.cpp
    src/ascrollarea.cpp
    src/ascrollbar.cpp
    src/asearchbar.cpp
    src/asegmentedbutton.cpp
    src/asettingrow.cpp
    src/aslider.cpp
    src/aswitch.cpp
    src/atoast.cpp
)
add_library(Arrera::Qt ALIAS arrera_qt)

set_target_properties(arrera_qt PROPERTIES
    EXPORT_NAME Qt
    POSITION_INDEPENDENT_CODE ON
)

target_compile_features(arrera_qt PUBLIC cxx_std_17)

target_include_directories(arrera_qt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
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
    OUTPUT_TARGETS arrera_qt_resource_targets
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

if(arrera_qt_resource_targets)
    set_target_properties(${arrera_qt_resource_targets} PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif()

if(ARRERA_QT_INSTALL)
    install(TARGETS arrera_qt ${arrera_qt_resource_targets}
        EXPORT arrera_qtTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/arrera_qt"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    install(EXPORT arrera_qtTargets
        FILE arrera_qtTargets.cmake
        NAMESPACE Arrera::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/arrera_qt"
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arrera_qtConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/arrera_qtConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/arrera_qt"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/arrera_qtConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/arrera_qtConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/arrera_qtConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/arrera_qt"
    )
endif()

if(ARRERA_QT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(ARRERA_QT_BUILD_DESIGNER_PLUGIN)
    add_subdirectory(designer)
endif()
