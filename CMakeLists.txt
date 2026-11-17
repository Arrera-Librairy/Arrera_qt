cmake_minimum_required(VERSION 3.16)
project(arrera_qt_test VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui Widgets)

qt_standard_project_setup()

qt_add_executable(arrera_qt_test WIN32 MACOSX_BUNDLE
    arrera_qt/aapplicationbar.cpp arrera_qt/aapplicationbar.h
    arrera_qt/apushbutton.cpp arrera_qt/apushbutton.h
    arrera_qt/arrera_theme.cpp arrera_qt/arrera_theme.h
    arrera_qt/asearchbar.cpp arrera_qt/asearchbar.h
    arrera_qt/roundedframe.cpp arrera_qt/roundedframe.h
    main.cpp
    test_ui.cpp test_ui.h test_ui.ui
)
target_include_directories(arrera_qt_test PRIVATE
    arrera_qt
)

target_compile_definitions(arrera_qt_test PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(arrera_qt_test PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS arrera_qt_test
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET arrera_qt_test
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
