cmake_minimum_required(VERSION 3.22)
project(data-logger VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Define source and include directories
set(SRC_DIR "src")
set(INCLUDE_DIR "include")
set(BIN_DIR "build")

# Find required packages
find_package(spdlog REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Add datetimelib as a subdirectory
add_subdirectory(../datetimelib ${CMAKE_BINARY_DIR}/datetimelib)

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

# Libraries for dependencies
set(DEPENDENT_LIBRARIES
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    ${BROTLI_COMMON_LIBRARY}
    ${BROTLI_ENC_LIBRARY}
    ${BROTLI_DEC_LIBRARY}
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    pthread
    datetimelib_static
)

# Common sources for executables
set(COMMON_SOURCES
    ${SRC_DIR}/cli.cpp
    ${SRC_DIR}/cfgsvc.cpp
    ${SRC_DIR}/logging.cpp
    ${SRC_DIR}/temperature.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/nodes.cpp
    ${SRC_DIR}/taskrunner.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/service.cpp
    ${SRC_DIR}/webhandlers.cpp
)

# Create executables
add_executable(data-logger
    ${COMMON_SOURCES}
    ${SRC_DIR}/main.cpp)

add_executable(data-tasks
    ${COMMON_SOURCES}
    ${SRC_DIR}/data-tasks.cpp)

add_executable(datalogger-unit 
    ${COMMON_SOURCES}
    ${SRC_DIR}/unit.cpp)

add_executable(datalogger-integration 
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/integration.cpp)

# if(APPLE)
 #    link_libraries(c++fs)
# endif()

# Configure all targets
foreach(target data-logger data-tasks datalogger-unit datalogger-integration)
    target_include_directories(${target} PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/vendor
        ${CMAKE_SOURCE_DIR}/../datetimelib/include
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

# Add stdc++fs to DEPENDENT_LIBRARIES
# list(APPEND DEPENDENT_LIBRARIES stdc++fs)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

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

Include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(tests tests/test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
