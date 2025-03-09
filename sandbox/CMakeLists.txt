cmake_minimum_required(VERSION 3.22)
project(tests)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include the main project's include directory
include_directories(${CMAKE_SOURCE_DIR}/../include)
include_directories(${CMAKE_SOURCE_DIR}/../include/app)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/../include/vendor)

# Dependencies
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++23" COMPILER_SUPPORTS_CXX23)
if(NOT COMPILER_SUPPORTS_CXX23)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++23 support.")
endif()

# Define source and include directories
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
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    message(STATUS "linux")
endif()


# Brotli (assumes installation via Homebrew or similar package manager)
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

set(CPM_DOWNLOAD_VERSION 0.27.2) 
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

get_property(dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
message(STATUS "!!!! Include directories: ${dirs}")


# ----------- spdlog 
CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.15.0)

# ----------- toml
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
)

# Compiler definitions
add_definitions(
    -DCPPHTTPLIB_OPENSSL_SUPPORT
    -DCPPHTTPLIB_ZLIB_SUPPORT
    -DCPPHTTPLIB_BROTLI_SUPPORT
)

# Collect all test files
file(GLOB TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

foreach(TEST_FILE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE ${DEPENDENT_LIBRARIES})
endforeach()

