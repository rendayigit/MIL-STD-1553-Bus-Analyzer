# Initialization
cd "$(dirname "$0")"
SCRIPTDIR=`pwd`
cd -

mkdir -p $SCRIPTDIR/../build/

# Default build type
BUILD_TYPE="Debug"

# Parse args for build mode (-r/--release or -d/--debug)
usage() {
	echo "Usage: $0 [-d|--debug] [-r|--release]"
	echo "If no option is provided, Debug mode is used."
}

while [ $# -gt 0 ]; do
	case "$1" in
		-r|--release)
			BUILD_TYPE="Release";
			shift
			;;
		-d|--debug)
			BUILD_TYPE="Debug";
			shift
			;;
		-h|--help)
			usage; exit 0
			;;
		*)
			echo "Unknown option: $1"; usage; exit 1
			;;
	esac
done

echo "Build mode: $BUILD_TYPE"

# Configure Project
cmake \
-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
-DCMAKE_BUILD_TYPE:STRING=${BUILD_TYPE} \
-DCMAKE_CC_COMPILER:FILEPATH=/usr/bin/gcc \
-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ \
-S$SCRIPTDIR/../ \
-B$SCRIPTDIR/../build/ \
--no-warn-unused-cli \
-G "Unix Makefiles"

# Build
cmake \
--build $SCRIPTDIR/../build/ \
--config ${BUILD_TYPE} \
--target all \
-j$((`nproc`)) --
