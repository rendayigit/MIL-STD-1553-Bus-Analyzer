# Initialization
cd `dirname $0`
SCRIPTDIR=`pwd`
cd -

LD_LIBRARY_PATH=$SCRIPTDIR:$SCRIPTDIR/../deps/wxWidgets/lib/:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH

# Run application
cd $SCRIPTDIR/../bin/
./bus-controller
