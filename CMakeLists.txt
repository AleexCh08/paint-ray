cmake_minimum_required(VERSION 3.15...4.3)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE)

project(RaylibPaint)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

if(WIN32)
    string(REPLACE "-mconsole" "-mwindows" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif()


message(STATUS "Descargando raygui.h...")
file(DOWNLOAD "https://raw.githubusercontent.com/raysan5/raygui/4.0/src/raygui.h" "${CMAKE_SOURCE_DIR}/src/raygui.h")


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS}
    src/shapes/BezierCurve.cpp
    src/models/PaintDocument.cpp
    src/views/PaintView.h
    src/views/PaintView.cpp
    src/controllers/PaintController.h
    src/controllers/PaintController.cpp
    src/tinyfiledialogs.c
    src/core/SplashScreen.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE "-mconsole")
endif()