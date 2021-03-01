cmake_minimum_required(VERSION 3.5)

project(test1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 COMPONENTS Widgets REQUIRED)
# find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)
find_package(Qt6 COMPONENTS OpenGL)
find_package(Qt6 COMPONENTS OpenGLWidgets)
find_package(Qt6 COMPONENTS Quick)
find_package(Qt6 COMPONENTS Qml)
find_package(Qt6 COMPONENTS QuickWidgets)
find_package(Qt6 COMPONENTS GuiPrivate)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        window.cpp
        window.h
        mainwindow.ui
        TheOpenGlWidget.cpp
        dialog.cpp
        geometryengine.cpp
        gameoflifemodel.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(test1
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(test1 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(test1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(test1 PRIVATE
    Qt::Widgets
    Qt::Core
    Qt::Gui
    Qt::OpenGL
    Qt::OpenGLWidgets
    Qt::Qml
    Qt::Quick
    Qt::QuickWidgets
    Qt::GuiPrivate
    )

set(shaders_resource_files
    "fshader.glsl"
    "vshader.glsl"
)

qt6_add_resources(test1 "shaders"
    PREFIX
        "/"
    FILES
        ${shaders_resource_files}
)
set(textures_resource_files
    "cube.png"
)


# Resources:
set_source_files_properties("./qt256.png"
    PROPERTIES QT_RESOURCE_ALIAS "qt256.png"
)
set_source_files_properties("./texture.frag.qsb"
    PROPERTIES QT_RESOURCE_ALIAS "texture.frag.qsb"
)
set_source_files_properties("./texture.vert.qsb"
    PROPERTIES QT_RESOURCE_ALIAS "texture.vert.qsb"
)
set(texuploads_resource_files
    "./qt256.png"
    "./texture.frag.qsb"
    "./texture.vert.qsb"
)

qt6_add_resources(test1 "texuploads"
    PREFIX
        "/"
    FILES
        ${texuploads_resource_files}
)


qt6_add_resources(test1 "textures"
    PREFIX
        "/"
    FILES
        ${textures_resource_files}
)

set(qmake_immediate_resource_files
    "gosperglidergun.cells"
    "main.qml"
)

qt6_add_resources(test1 "qmake_immediate"
    PREFIX
        "/"
    FILES
        ${qmake_immediate_resource_files}
)

# install(TARGETS test1
#     RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
#     BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
#     LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
# )

set_target_properties(test1 PROPERTIES
    QT_QML_MODULE_VERSION 1.0
    QT_QML_MODULE_URI GameOfLifeModel
)
set_target_properties(test1 PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

qt6_qml_type_registration(test1)
