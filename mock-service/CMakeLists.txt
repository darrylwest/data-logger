cmake_minimum_required(VERSION 3.22)

project(MockServiceTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include main project directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(SYSTEM ${CMAKE_SOURCE_DIR}/include/vendor)

# Dependencies
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

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

# Ensure CPM is downloaded
set(CPM_DOWNLOAD_VERSION 0.27.2)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.15.0)

CPMAddPackage(
    NAME toml11
    GITHUB_REPOSITORY "ToruNiina/toml11"
    VERSION 4.2.0
    OPTIONS "TOML11_PRECOMPILE ON"
)

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

add_definitions(
    -DCPPHTTPLIB_OPENSSL_SUPPORT
    -DCPPHTTPLIB_ZLIB_SUPPORT
    -DCPPHTTPLIB_BROTLI_SUPPORT
)

# Mock-service source files
set(MOCK_SERVICE_SOURCES
    client-fetch.cpp
    json-config.cpp
    mock_service.cpp
    retry-service.cpp
    struct-enum.cpp
    succeed-fail.cpp
    task-runner.cpp
    truncate-minutes.cpp
)

# Create executables for each mock service file
foreach(SRC ${MOCK_SERVICE_SOURCES})
    get_filename_component(EXE_NAME ${SRC} NAME_WE)
    add_executable(${EXE_NAME} ${SRC})
    target_link_libraries(${EXE_NAME} PRIVATE ${DEPENDENT_LIBRARIES})
endforeach()

# Output binaries to the build directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/mock-service)

if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    list(APPEND DEPENDENT_LIBRARIES
        "-framework CoreFoundation"
        "-framework Security"
    )
endif()

