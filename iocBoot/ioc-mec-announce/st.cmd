#!/bin/sh
export PSPKG_ROOT=/reg/g/pcds/pkg_mgr
export PSPKG_RELEASE=controls-0.0.1
source $PSPKG_ROOT/etc/set_env.sh
./annunciator.py "$@"
