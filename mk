#!/usr/bin/env bash
#
# dpw@plaza.localdomain | 2024-12-13 03:28:43
#
#

set -eu

export root=`pwd`

project=data-logger

# parse the cli
while [[ $# -gt 0 ]]
do
    case $1 in
        init)
            /bin/rm -fr build/
            [ -d dep-cache ] || mkdir dep-cache
            (mkdir build && cd build && cmake -DCPM_SOURCE_CACHE=../dep-cache ..)

            shift
        ;;
        build)
            clear

            # remove any old unit test
            /bin/rm -f $root/build/unit

            # cmake --build build/ 
            (cd build && make -j4)

            # $root/build/$project --version
            # ln $root/build/cryptor $root/build/unit

            shift
        ;;
        unit)
            (cd build && make -j4)
            $root/build/unit

            shift
        ;;
        run)
            (cd build && make -j4)
            $root/build/$project

            shift
        ;;
        format)
            clang-format -i include/app/*.hpp src/*.cpp
            git status -s

            shift
        ;;
        clean)
            (cd build && make clean && /bin/rm -f unit)
        
            shift
        ;;
        clobber)
            /bin/rm -fr build/

            shift
        ;;
        watch)
            watchexec -c -w src/ -w include/ -e h,hpp,cpp ./mk build test

            exit 0
        ;;

        pull)
            git pull

            shift
        ;;

        help)
            echo "Targets:"
            echo ""
            echo "   init     : run the cmake command to create the build folder"
            echo "   build    : compile cryptor to the build folder"
            echo "   run      : runs the app and shows version"
            echo "   format   : runs clang-format over includes and src"
            echo "   watch    : run watcher over source and include"
            echo "   pull     : pull the latest repo changes"
            echo "   clean    : remove binary builds but leave the build folder"
            echo "   clobber  : remove the entire build folder"
            echo "   help     : show this help"
            
            exit 0
        ;;

        build)
            cd src && pwd && make && make unit

            shift
        ;;
        *)
            ./mk help
            exit 0
        ;;

    esac
done


