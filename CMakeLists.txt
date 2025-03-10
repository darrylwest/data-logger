cmake_minimum_required(VERSION 3.3)
project(data-logger VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Add minimum compiler version check
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 16)
        message(FATAL_ERROR "Clang version must be at least 16!")
    endif()
endif()

# Add more build type validation
set(ALLOWED_BUILD_TYPES Debug Release RelWithDebInfo MinSizeRel)
if(NOT CMAKE_BUILD_TYPE IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
endif()

# Useful for IDE integration and static analysis tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")

# Add version checks for system requirements
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++23" COMPILER_SUPPORTS_CXX23)
if(NOT COMPILER_SUPPORTS_CXX23)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++23 support.")
endif()

# Define source and include directories
set(SRC_DIR "src")
set(INCLUDE_DIR "include")
set(BIN_DIR "build")

set(CPM_DOWNLOAD_VERSION 0.27.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

# Find required packages
CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.15.0)

# find_package(spdlog 1.15.2 REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL 3.0 REQUIRED)
find_package(ZLIB 1.2.11 REQUIRED)

# Add datetimelib as a subdirectory
# add_subdirectory(datetimelib ${CMAKE_BINARY_DIR}/datetimelib)
# Find datetimelib
find_path(DATETIMELIB_INCLUDE_DIR NAMES datetimelib/include/datetimelib/datetime.hpp)
# find_library(DATETIMELIB_LIBRARY NAMES datetimelib/build)

# MacOS universal binary setup
if(APPLE)
    execute_process(
            COMMAND uname -m
            OUTPUT_VARIABLE HOST_ARCHITECTURE
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if(HOST_ARCHITECTURE STREQUAL "arm64")
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    else()
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()
    list(APPEND DEPENDENT_LIBRARIES
            "-framework CoreFoundation"
            "-framework Security"
    )
    message(STATUS "osx arch: ${CMAKE_OSX_ARCHITECTURES}")
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_SPECIFIC_LIBS
        # Ubuntu-specific libraries if needed
    )
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    message(STATUS "linux")
endif()

find_path(BROTLI_INCLUDE_DIR brotli PATHS ${BROTLI_ROOT}/include)
find_library(BROTLI_COMMON_LIBRARY NAMES brotlicommon PATHS ${BROTLI_ROOT}/lib)
find_library(BROTLI_ENC_LIBRARY NAMES brotlienc PATHS ${BROTLI_ROOT}/lib)
find_library(BROTLI_DEC_LIBRARY NAMES brotlidec PATHS ${BROTLI_ROOT}/lib)

if(NOT BROTLI_INCLUDE_DIR OR NOT BROTLI_COMMON_LIBRARY OR NOT BROTLI_ENC_LIBRARY OR NOT BROTLI_DEC_LIBRARY)
    message(FATAL_ERROR "Brotli libraries not found. Make sure they are installed.")
endif()

# unit test lib catch2
Include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

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
    ${CMAKE_SOURCE_DIR}/datetimelib/build/libdatetimelib.a
    Catch2::Catch2WithMain
)

# Common sources for executables
set(COMMON_SOURCES
    ${SRC_DIR}/cli.cpp
    ${SRC_DIR}/cfgsvc.cpp
    ${SRC_DIR}/temperature.cpp
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/nodes.cpp
    ${SRC_DIR}/taskrunner.cpp
    ${SRC_DIR}/client.cpp
    ${SRC_DIR}/service.cpp
    ${SRC_DIR}/webhandlers.cpp
    ${SRC_DIR}/http_client.cpp
)

# Create executables
add_executable(data-logger
    ${COMMON_SOURCES}
    ${SRC_DIR}/main.cpp
    tests/version_tests.cpp)

add_executable(data-tasks
    ${COMMON_SOURCES}
    ${SRC_DIR}/data-tasks.cpp)

add_executable(datalogger-unit 
    ${COMMON_SOURCES}
    ${SRC_DIR}/unit.cpp)

add_executable(datalogger-integration 
    ${SRC_DIR}/database.cpp
    ${SRC_DIR}/integration.cpp)

add_executable(unit_tests
    ${COMMON_SOURCES}
    tests/database_tests.cpp
    tests/taskrunner_tests.cpp
    tests/temperature_tests.cpp
    tests/main_unit.cpp
    tests/cfgsvc_tests.cpp
    tests/client_tests.cpp
    tests/cli_tests.cpp
    tests/exceptions_tests.cpp
    tests/webhandlers_tests.cpp
    tests/service_tests.cpp
    tests/datetimelib_tests.cpp
    tests/version_tests.cpp
)

# Add optimization flags based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(data-logger PRIVATE -O3)
    target_compile_options(data-tasks PRIVATE -O3)
    # Add other optimization flags as needed
endif()

# Specify include directories for tests
# target_include_directories(unit_tests PRIVATE src include include/app include/vendor)
# target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain )

# Configure all targets
# Create a function for setting up common target properties
function(setup_target_properties target)
    target_include_directories(${target} PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include/vendor
            ${CMAKE_SOURCE_DIR}/datetimelib/include
    )
    target_link_libraries(${target} PRIVATE ${DEPENDENT_LIBRARIES})
    target_compile_options(${target} PRIVATE -Wall -Wextra -Werror)
    set_target_properties(${target} PROPERTIES UNITY_BUILD ON)
    # Add precompiled header support
    target_precompile_headers(${target} PRIVATE ${CMAKE_SOURCE_DIR}/include/precompiled.hpp)
endfunction()

# Use the function for each target
foreach(target data-logger data-tasks datalogger-unit datalogger-integration unit_tests)
    setup_target_properties(${target})
endforeach()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/${BIN_DIR})

if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# Apply unity build settings consistently
set(UNITY_BUILD_TARGETS
        data-logger
        data-tasks
        datalogger-unit
        datalogger-integration
        unit_tests
)

foreach(target ${UNITY_BUILD_TARGETS})
    set_target_properties(${target} PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_MAX_FILE_COUNT 4
            UNITY_BUILD_BATCH_SIZE 0
    )
endforeach()

# Enable testing
enable_testing()

# Add tests
add_test(NAME unit_tests COMMAND unit_tests)

# Add installation rules
install(TARGETS data-logger data-tasks datalogger-unit datalogger-integration
        RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING
            PATTERN "*.hpp"
            PATTERN "*.h"
)

