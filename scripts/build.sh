# Initialization
cd `dirname $0`
SCRIPTDIR=`pwd`
cd -

mkdir -p $SCRIPTDIR/../build/

# Configure Project
cmake \
-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
-DCMAKE_BUILD_TYPE:STRING=Debug \
-DCMAKE_CC_COMPILER:FILEPATH=/usr/bin/gcc \
-DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ \
-DSLINT_STYLE="fluent" \
-S$SCRIPTDIR/../ \
-B$SCRIPTDIR/../build/ \
-G "Unix Makefiles" \
--no-warn-unused-cli

# Build
cmake \
--build $SCRIPTDIR/../build/ \
--config Debug \
--target all \
-j$((`nproc`+2)) --