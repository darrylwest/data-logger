
include(FetchContent)

find_package(datetimelib CONFIG QUIET)

if(NOT datetimelib_FOUND)
    FetchContent_Declare(
        datetimelib
        GIT_REPOSITORY https://github.com/darrylwest/datetimelib.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(datetimelib)

    # Create an alias target to match the expected name
    if(NOT TARGET datetimelib::datetimelib)
        add_library(datetimelib::datetimelib ALIAS datetimelib_static)
    endif()
endif()
