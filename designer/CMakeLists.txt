find_package(Qt6 REQUIRED COMPONENTS UiPlugin)

add_library(arrera_qt_designer_plugin SHARED
    arrerawidgetcollection.h
    arrerawidgetcollection.cpp
)

target_compile_definitions(arrera_qt_designer_plugin PRIVATE
    QT_PLUGIN
    QT_DEPRECATED_WARNINGS
)

target_link_libraries(arrera_qt_designer_plugin PRIVATE
    Arrera::Qt
    Qt::UiPlugin
)

if(DEFINED QT6_INSTALL_PLUGINS)
    set(_arrera_designer_dest "${QT6_INSTALL_PLUGINS}/designer")
else()
    set(_arrera_designer_dest "${CMAKE_INSTALL_LIBDIR}/qt6/plugins/designer")
endif()

install(TARGETS arrera_qt_designer_plugin
    LIBRARY DESTINATION "${_arrera_designer_dest}"
    RUNTIME DESTINATION "${_arrera_designer_dest}"
)
