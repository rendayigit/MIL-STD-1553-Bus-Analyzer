# Initialization
cd `dirname $0`
SCRIPTDIR=`pwd`
cd -

# Make Clean
cmake \
--build $SCRIPTDIR/../build/ \
--target clean

# Force Delete Dirs
rm -rf $SCRIPTDIR/../bin/
rm -rf $SCRIPTDIR/../build/

# Force Delete Residuals
rm -rf $SCRIPTDIR/../cmake_install.cmake
rm -rf $SCRIPTDIR/../CMakeCache.txt
rm -rf $SCRIPTDIR/../Makefile