qt_add_executable(arrera_qt_test WIN32 MACOSX_BUNDLE
    main.cpp
    test_ui.cpp
    test_ui.h
    test_ui.ui
)

target_include_directories(arrera_qt_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(arrera_qt_test PRIVATE
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(arrera_qt_test PRIVATE
    Arrera::Qt
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
