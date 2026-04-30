cmake_minimum_required(VERSION 3.16)

project(homeAssistent VERSION 0.1 LANGUAGES CXX)

# Включаем автоматическую обработку файлов Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Используем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем Qt6 или Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

# Ищем SQLite3
find_package(SQLite3 REQUIRED)

# Основные исходные файлы
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    DataBase.h
    DataBase.cpp
    BaseWgt.h
    BaseWgt.cpp
    BaseWgt.ui
)

# Дополнительные файлы интерфейса (только для ПК)
list(APPEND PROJECT_SOURCES
    Notes.h Notes.cpp Notes.ui
    WhetherWgt.h WhetherWgt.cpp WhetherWgt.ui
    PcStats.h PcStats.cpp PcStats.ui
)

# Создаём исполняемый файл
if(Qt6_FOUND OR QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(homeAssistent
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(homeAssistent ${PROJECT_SOURCES}
        MonitorWgt.h MonitorWgt.cpp MonitorWgt.ui
        )
endif()

# Подключаем основные библиотеки
target_link_libraries(homeAssistent PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    SQLite::SQLite3
)

target_include_directories(homeAssistent PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Платформенно-зависимые библиотеки
if(WIN32)
    # Библиотеки Windows
    target_link_libraries(homeAssistent PRIVATE
        dxva2          # Для управления яркостью монитора (DDC/CI)
        ole32          # Для COM (CoInitialize и др.)
        mmdevapi       # Для IAudioEndpointVolume (громкость)
        user32         # Для SendMessage (выключение экрана)
        winmm          # Опционально, если будешь использовать waveOutSetVolume
    )
    target_compile_definitions(homeAssistent PRIVATE OS_WINDOWS)
elseif(UNIX AND NOT APPLE)
    # Linux
    target_link_libraries(homeAssistent PRIVATE
        pthread     # Потоки
        dl          # Динамическая загрузка библиотек
    )
    target_compile_definitions(homeAssistent PRIVATE OS_LINUX)
endif()

# Настройки macOS (если вдруг соберёте на Mac)
if(APPLE)
    target_compile_definitions(homeAssistent PRIVATE OS_MACOS)
endif()

# Настройки для macOS/iOS bundle (если Qt < 6.1)
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.homeAssistent)
endif()

# Устанавливаем свойства исполняемого файла
set_target_properties(homeAssistent PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Установка: копирование бинарника в bin/
include(GNUInstallDirs)
install(TARGETS homeAssistent
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# Финализация для Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(homeAssistent)
endif()
