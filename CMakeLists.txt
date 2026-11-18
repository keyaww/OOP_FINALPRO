cmake_minimum_required(VERSION 3.16)
project(Billiards CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Find SFML 3 (installed via Homebrew) ──────────────────────────────────────
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# ── Sources ───────────────────────────────────────────────────────────────────
set(SOURCES
    src/main.cpp
    src/Ball.cpp
    src/CueStick.cpp
    src/Table.cpp
    src/Physics.cpp
    src/Game.cpp
)

add_executable(billiards ${SOURCES})

target_link_libraries(billiards PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)

# macOS: embed app bundle-compatible RPATH
if(APPLE)
    set_target_properties(billiards PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path"
    )
endif()
