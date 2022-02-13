#!/bin/bash

ACTION=$1

case $ACTION in

	init)
		cmake -E make_directory "$BUILD_DIR"
		echo === Building EMAWP ============================================
		git clone https://github.com/jakubfi/emawp
		cmake -E make_directory emawp/build
		cmake emawp -G "Unix Makefiles" -B emawp/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/mingw64
		cmake --build emawp/build
		cmake --install emawp/build
		echo === Building EMAS ============================================
		git clone https://github.com/jakubfi/emas
		cmake -E make_directory emas/build
		cmake emas -G "Unix Makefiles" -B emas/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/mingw64
		cmake --build emas/build
		cmake --install emas/build
	;;

	configure)
		cmake "$SRC_DIR" -G "Unix Makefiles" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"
	;;

	build)
		cmake --build "$BUILD_DIR"
	;;

	install)
		cmake --install "$BUILD_DIR"
	;;

	test)
		cd "$SRC_DIR"/tests
		./runtests.sh
	;;

	*)
		echo "Unknown action: $ACTION"
		exit 1
	;;

esac

# vim: tabstop=4
