# Initialization
cd "$(dirname "$0")"
SCRIPTDIR=`pwd`
cd -

# Run unit tests
cd $SCRIPTDIR/../bin/
./bus-monitor
