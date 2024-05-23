# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(sensorsshowcase LANGUAGES CXX)

# Set the example installation directory
if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()
set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/sensors/sensorsshowcase")

# Find necessary Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Svg Sensors Charts)
qt_standard_project_setup(REQUIRES 6.5)

# Add the executable
qt_add_executable(sensorsshowcase
    main.cpp
)

# Set target properties
set_target_properties(sensorsshowcase PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Android specific settings
if(ANDROID)
    set_property(TARGET sensorsshowcase PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        ${CMAKE_CURRENT_SOURCE_DIR}/android)
endif()

# iOS specific settings
if(APPLE AND IOS)
    set_property(TARGET sensorsshowcase PROPERTY
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
    )
else()
    # Default search path for the config file is "qrc:/"
    qt_add_resources(sensorsshowcase "resources"
        FILES qtquickcontrols2.conf)
endif()

# Add QML module
qt_add_qml_module(sensorsshowcase
    URI SensorShowcaseModule
    VERSION 1.0
    QML_FILES
        "Main.qml"
    RESOURCES
        # Uncomment and add resource files as needed
        # "images/compass.svg"
        # "images/magnet.svg"
        # "images/qt_logo.png"
    SOURCES
        sensorsupport.h
        accelerometersensor.h accelerometersensor.cpp
        gyroscopesensor.h gyroscopesensor.cpp
        validator.h validator.cpp
)

# Link necessary libraries
target_link_libraries(sensorsshowcase
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Quick
        Qt::Svg
        Qt::Sensors
        Qt::Charts
)

# Install the target
install(TARGETS sensorsshowcase
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)

# Use precompiled headers for faster builds
target_precompile_headers(sensorsshowcase PRIVATE <QtCore> <QtGui> <QtQuick> <QtSvg> <QtSensors> <QtCharts>)

# Enable faster builds with Ninja
if (CMAKE_GENERATOR STREQUAL "Ninja")
    message(STATUS "Using Ninja generator")
endif()

# Set parallel build options
if (CMAKE_BUILD_PARALLEL_LEVEL)
    message(STATUS "Parallel builds enabled with ${CMAKE_BUILD_PARALLEL_LEVEL} jobs")
endif()
