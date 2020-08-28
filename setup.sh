#!/bin/sh
#
# Setup the erep-sim directory for development (or simply running the
# simulation).  This makes sure that the ROOT environment variables
# are set (using thisroot.sh), and then defines a couple of
# conveniences commands:
#
#  erep-build == Source ./build/erep-build.sh which will conveniently
#           run cmake/make/make install from any place so that it's
#           really easy to recompile.
#
#  erep-setup == Source this file.  You probably never have to use
#           this one.
#
# This setup script is not needed.  You can also do it by hand.  It's
# a usual cmake build, but you need to make sure root and geant are
# "in the path".
#
# source thisroot.sh
# cd the-build-directory
# cmake -DCMAKE_INSTALL_PREFIX=the-install-directory the-erep-sim-directory 
# make
# make install


# Try to setup root.  ROOT installs thisroot.sh in the bin directory
# to setup the environment.  The first "thisroot.sh" in the path will
# define the root that is used.
. thisroot.sh >& /dev/null
if [ $? != 0 ]; then
    echo ROOT not available.
fi

# Find the root of the building area.
___erep_root() {
    COUNT=50
    while true; do
	if [ -e ./build -a -d ./build -a -e ./build/erep-build.sh ]; then
	    echo ${PWD}
	    return
	fi
	COUNT=$(expr ${COUNT} - 1)
	if [ ${COUNT} -lt 1 ]; then
	    echo invalid-directory
	    return
	fi
	cd ..
    done
}

export EREP_ROOT
EREP_ROOT=$(___erep_root)
unset -f ___erep_root

if [ ${EREP_ROOT} = "invalid-directory" ]; then
    echo EREP-SIM setup.sh must be sourced in erep-sim directory.
    return
fi

___erep_target () {
    target="erep"
    compiler=gcc
    target="${target}-${compiler}-$(cc -dumpversion)-$(cc -dumpmachine)"
    echo $target
}

export EREP_TARGET
EREP_TARGET=$(___erep_target)
unset -f ___erep_target

___path_prepend () {
    ___path_remove $1 $2
    eval export $1="$2:\$$1"
}
___path_remove ()  {
    export $1=$(eval echo -n \$$1 | \
	awk -v RS=: -v ORS=: '$0 != "'$2'"' | \
	sed 's/:$//'); 
}

___path_prepend PATH ${EREP_ROOT}/${EREP_TARGET}/bin
___path_prepend LD_LIBRARY_PATH ${EREP_ROOT}/${EREP_TARGET}/lib

unset -f ___path_prepend
unset -f ___path_remove


alias erep-setup=". ${EREP_ROOT}/setup.sh"

alias erep-build="${EREP_ROOT}/build/erep-build.sh"

echo Defined erep-setup to re-setup the erep-sim package.
echo Defined erep-build to build the the erep-sim package.
