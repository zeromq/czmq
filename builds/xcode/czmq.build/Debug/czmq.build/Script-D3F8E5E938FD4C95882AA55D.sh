#!/bin/sh
make -C /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode -f /Users/steph/Documents/CloudStation/ingescape/code/czmq/builds/xcode/CMakeScripts/czmq_postBuildPhase.make$CONFIGURATION OBJDIR=$(basename "$OBJECT_FILE_DIR_normal") all
