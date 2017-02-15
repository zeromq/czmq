#!/usr/bin/env bash
set -ex

# NOTE: This script is not standalone, it is included from project root
# ci_build.sh script, which sets some envvars (like REPO_DIR below).
[ -n "${REPO_DIR-}" ] || exit 1

# NOTE: Only build docs on Non-PR build
if [[ $TRAVIS_PULL_REQUEST == 'false' ]]; then
    echo "Skip docs on PR"
    exit 0
fi

# Checkout the tools to build the API
cd "$REPO_DIR/.."
git clone --quiet --depth 1 https://github.com/zeromq/ztools.git ztools

# Export upload script
cd ztools/wdput
export PATH=$PATH:$PWD

# Build the API for master
cd ../czmqapi
./apione ../../czmq       master   CZMQ-MASTER
