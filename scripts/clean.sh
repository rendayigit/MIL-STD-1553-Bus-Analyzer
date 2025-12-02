# Initialization
cd "$(dirname "$0")"
SCRIPTDIR=`pwd`
cd -

# Make Clean
cmake \
--build $SCRIPTDIR/../build/ \
--target clean

# Force Delete Dirs
rm -rf $SCRIPTDIR/../bin/
rm -rf $SCRIPTDIR/../build/
rm -rf $SCRIPTDIR/../.cache/