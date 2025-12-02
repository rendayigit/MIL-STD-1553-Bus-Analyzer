cmake_minimum_required(VERSION 3.21)
project(bus-controller LANGUAGES C CXX)

# Set target directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)

# Find boost installation
find_package(Boost 1.74.0 COMPONENTS filesystem REQUIRED)

# Find wxWidgets using wx-config (more reliable for older versions)
find_program(WX_CONFIG wx-config)
if(NOT WX_CONFIG)
    message(FATAL_ERROR "wx-config not found. Please install wxWidgets development packages.")
endif()

# Get wxWidgets version
execute_process(COMMAND ${WX_CONFIG} --version OUTPUT_VARIABLE WX_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Found wxWidgets version: ${WX_VERSION}")

# Get wxWidgets flags
execute_process(COMMAND ${WX_CONFIG} --cxxflags OUTPUT_VARIABLE WX_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${WX_CONFIG} --libs OUTPUT_VARIABLE WX_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Convert space-separated flags to list
separate_arguments(WX_CXX_FLAGS)
separate_arguments(WX_LIBS)

# Include SourceFiles.cmake to access the SOURCEFILES variable
include(${CMAKE_CURRENT_LIST_DIR}/SourceFiles.cmake)

add_executable(bus-controller ${SOURCEFILES})

# Apply wxWidgets flags
target_compile_options(bus-controller PRIVATE ${WX_CXX_FLAGS})

target_link_libraries(bus-controller PRIVATE
    ${WX_LIBS}
    Boost::filesystem
    fmt
    spdlog
    ${CMAKE_SOURCE_DIR}/deps/ddc-driver/lib/libemacepl.a
)

target_include_directories(bus-controller PUBLIC
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/ddc-driver/include/emacepl
    ${CMAKE_SOURCE_DIR}/deps/ddc-driver/include/ddcacex
)