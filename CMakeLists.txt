cmake_minimum_required(VERSION 3.21)
project(bm-gui LANGUAGES C CXX)

# Set target directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin/)

# Find boost installation
find_package(Boost 1.74.0 COMPONENTS filesystem REQUIRED)

# Find spdlog installation
find_package(spdlog REQUIRED)

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

add_executable(bm-gui
    ${CMAKE_SOURCE_DIR}/ui/mainWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/app.cpp
    ${CMAKE_SOURCE_DIR}/src/bm.cpp
    ${CMAKE_SOURCE_DIR}/src/logger.cpp
)

target_compile_definitions(bm-gui PUBLIC
    _FILE_OFFSET_BITS=64 WXUSINGDLL __WXGTK__
)

target_link_libraries(bm-gui PRIVATE
    Boost::filesystem
    spdlog::spdlog
    ${CMAKE_SOURCE_DIR}/deps/wxWidgets/lib/libwx_baseu_net-3.2.so.0
    ${CMAKE_SOURCE_DIR}/deps/wxWidgets/lib/libwx_baseu-3.2.so.0
    ${CMAKE_SOURCE_DIR}/deps/wxWidgets/lib/libwx_gtk3u_core-3.2.so.0
    ${CMAKE_SOURCE_DIR}/deps/ddc-driver/lib/libemacepl.a
)

target_include_directories(bm-gui PUBLIC ${INCLUDEDIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/ui
    ${CMAKE_SOURCE_DIR}/deps/ddc-driver/include/emacepl
    ${CMAKE_SOURCE_DIR}/deps/ddc-driver/include/ddcacex
    ${CMAKE_SOURCE_DIR}/deps/wxWidgets/include
    ${CMAKE_SOURCE_DIR}/deps/wxWidgets/lib/wx/include/gtk3-unicode-3.2
)