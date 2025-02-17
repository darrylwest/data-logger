
include(FetchContent)

find_package(datetimelib CONFIG QUIET)

if(NOT datetimelib_FOUND)
    FetchContent_Declare(
        datetimelib
        GIT_REPOSITORY https://github.com/darrylwest/datetimelib.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(datetimelib)
endif()
