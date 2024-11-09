# Initialization
cd `dirname $0`
SCRIPTDIR=`pwd`
cd -

LD_LIBRARY_PATH=$SCRIPTDIR:$SCRIPTDIR/../deps/slint-1.8/lib/:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

# Run application
cd $SCRIPTDIR/../bin/
./bm-gui
