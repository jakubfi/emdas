#!/bin/bash

ACTION=$1

case $ACTION in

	init)
		cmake -E make_directory "$BUILD_DIR"
		git clone https://github.com/jakubfi/emawp
		cmake -E make_directory emawp/build
		cmake emawp -G "Unix Makefiles" -B emawp/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/mingw64
		cmake --build emawp/build
		cmake --install emawp/build
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
		cd "$BUILD_DIR"/tests
		./runtests.sh
	;;

	*)
		echo "Unknown action: $ACTION"
		exit 1
	;;

esac

# vim: tabstop=4
