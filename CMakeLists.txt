cmake_minimum_required(VERSION 3.24)
project(uav VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if (MSVC)
    add_compile_options(/utf-8)
    set(PATH_RC_VERSION "${CMAKE_BINARY_DIR}/version.rc")
    configure_file(version.rc.in ${PATH_RC_VERSION})

    set(PATH_H_VERSION "${CMAKE_BINARY_DIR}/version.h")
    configure_file(version.h.in ${PATH_H_VERSION})
    include_directories(${CMAKE_BINARY_DIR})
endif ()

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}
        "res/*.*"
        "ui/option/*"
        "ui/progress/*"
        "ui/test/*"
        "ui/main/*"
        "utils/*"
        )

find_package(Qt6 COMPONENTS Core Gui Widgets Xml LinguistTools REQUIRED)

set(TS_FILES ${CMAKE_SOURCE_DIR}/lang/zh_CN.ts)
qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_executable(uav main.cpp ${SOURCES} ${PATH_RC_VERSION} ${PATH_H_VERSION} ${TS_FILES} ${QM_FILES})
elseif (CMAKE_BUILD_TYPE MATCHES Release)
    add_executable(uav WIN32 main.cpp ${SOURCES} ${PATH_RC_VERSION} ${PATH_H_VERSION} ${TS_FILES} ${QM_FILES})
endif ()

target_link_libraries(uav PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Xml)