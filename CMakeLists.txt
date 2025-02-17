cmake_minimum_required(VERSION 3.22)

project(data-logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Add the main include directory 
include_directories(${CMAKE_SOURCE_DIR}/include) 

# Add the vendor include directory 
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include/vendor)

# Define source and include directories
set(SRC_DIR "src")
set(INCLUDE_DIR "include")
set(BIN_DIR "build")

# Add custom module path for FindDatetimelib
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Setup datetimelib dependency
include(FetchContent)
FetchContent_Declare(
    datetimelib
    GIT_REPOSITORY https://github.com/darrylwest/datetimelib.git
    GIT_TAG main  # or specify a specific tag/commit
)
FetchContent_MakeAvailable(datetimelib)

# Dependencies
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Brotli setup
if(APPLE AND EXISTS "/opt/homebrew")
    set(BROTLI_ROOT "/opt/homebrew")
else()
    set(BROTLI_ROOT "/usr/local")
endif()

find_path(BROTLI_INCLUDE_DIR brotli PATHS ${BROTLI_ROOT}/include)
find_library(BROTLI_COMMON_LIBRARY NAMES brotlicommon PATHS ${BROTLI_ROOT}/lib)
find_library(BROTLI_ENC_LIBRARY NAMES brotlienc PATHS ${BROTLI_ROOT}/lib)
find_library(BROTLI_DEC_LIBRARY NAMES brotlidec PATHS ${BROTLI_ROOT}/lib)

if(NOT BROTLI_INCLUDE_DIR OR NOT BROTLI_COMMON_LIBRARY OR NOT BROTLI_ENC_LIBRARY OR NOT BROTLI_DEC_LIBRARY)
    message(FATAL_ERROR "Brotli libraries not found. Make sure they are installed.")
endif()

# CPM setup for additional dependencies
set(CPM_DOWNLOAD_VERSION 0.27.2) 
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

# spdlog + fmt
CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.15.0)

# toml
CPMAddPackage(
    NAME toml11
    GITHUB_REPOSITORY "ToruNiina/toml11"
    VERSION 4.2.0
    OPTIONS "TOML11_PRECOMPILE ON"
)

# Libraries for dependencies
set(DEPENDENT_LIBRARIES
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    ${BROTLI_COMMON_LIBRARY}
    ${BROTLI_ENC_LIBRARY}
    ${BROTLI_DEC_LIBRARY}
    spdlog::spdlog
    toml11::toml11 
    nlohmann_json::nlohmann_json
    pthread
    datetimelib_static
)

# Create executables
add_executable(data-logger
    ${SRC_DIR}/cli.cpp
    ${SRC_DIR}/logging.cpp
    ${SRC_DIR}/temperature.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/nodes.cpp
    ${SRC_DIR}/taskrunner.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/service.cpp
    ${SRC_DIR}/main.cpp)

add_executable(data-tasks
    ${SRC_DIR}/cli.cpp
    ${SRC_DIR}/logging.cpp
    ${SRC_DIR}/temperature.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/nodes.cpp
    ${SRC_DIR}/taskrunner.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/service.cpp
    ${SRC_DIR}/data-tasks.cpp)

add_executable(datalogger-unit 
    ${SRC_DIR}/cli.cpp
    ${SRC_DIR}/logging.cpp
    ${SRC_DIR}/temperature.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/nodes.cpp
    ${SRC_DIR}/taskrunner.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/service.cpp
    ${SRC_DIR}/unit.cpp)

add_executable(datalogger-integration 
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/integration.cpp)

# Add include directories for datetimelib
foreach(target data-logger data-tasks datalogger-unit datalogger-integration)
    target_include_directories(${target} PRIVATE 
        ${datetimelib_SOURCE_DIR}/include
    )
    target_link_libraries(${target} PRIVATE ${DEPENDENT_LIBRARIES})
    target_compile_options(${target} PRIVATE -Wall -Wextra -Werror)
    set_target_properties(${target} PROPERTIES UNITY_BUILD ON)
endforeach()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${BIN_DIR})

if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

# MacOS universal binary setup
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    list(APPEND DEPENDENT_LIBRARIES
        "-framework CoreFoundation"
        "-framework Security"
    )
endif()

# Unity build settings
set_target_properties(data-logger PROPERTIES UNITY_BUILD_MAX_FILE_COUNT 4)
set_target_properties(data-tasks PROPERTIES UNITY_BUILD_MAX_FILE_COUNT 4)
