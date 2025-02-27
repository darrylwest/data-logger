#!/usr/bin/env bash
#
# dpw@plaza.localdomain | 2024-12-13 03:28:43
#
#

set -eu

export root=`pwd`

export project=data-logger
export port=9090

# parse the cli
while [[ $# -gt 0 ]]
do
    case $1 in
        init)
            mv build/_deps/ . && /bin/rm -fr build && mkdir build && mv _deps/ build/ 
            (cd build && cmake ..)

            shift
        ;;
        build)
            clear

            (cd build && time make -j8 || exit 1)

            shift
        ;;
        unit)
            (cd build && make -j8)
            $root/build/datalogger-unit
            $root/build/tests

            shift
        ;;
        test)
            (cd build && make -j8)
            $root/build/datalogger-integration

            shift
        ;;
        run)
            (cd build && make -j8)
            $root/build/$project

            shift
        ;;
        run-data)
            (cd build && make -j8)
            export TESTING=true
            $root/build/data-tasks

            shift
        ;;
        format)
            clang-format -i include/app/*.hpp src/*.cpp
            git status -s

            shift
        ;;
        clean)
            (cd build && make clean && /bin/rm -f datalogger-unit)
        
            shift
        ;;
        clobber)
            /bin/rm -fr build/

            shift
        ;;
        watch)
            watchexec -c -w src/ -w include/ -e h,hpp,cpp ./mk build unit

            exit 0
        ;;
        shutdown)
            curl -X DELETE "http://localhost:${port}/api/shutdown"
            shift
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
            echo "   run      : runs the web server app"
            echo "   run-data : runs the data-tasks app"
            echo "   format   : runs clang-format over includes and src"
            echo "   watch    : run watcher over source and include"
            echo "   pull     : pull the latest repo changes"
            echo "   clean    : remove binary builds but leave the build folder"
            echo "   clobber  : remove the entire build folder"
            echo "   shutdown : shutdown a local instance"
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


