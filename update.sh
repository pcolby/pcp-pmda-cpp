#!/bin/bash
#
# This is a convenient script used for copying doxygen-generated HTML from an
# out-of-source build directory to the current gh-pages branch. It's not meant
# for general consumption.
#

BUILD_DIR=../cpp/build
API_DIR=${BUILD_DIR}/include/api
GH_PAGES_DIR=.

# Regenerate doxygen's HTML output.
pushd ${BUILD_DIR}
make clean doc
popd

# Copy the generated output to the gh-pages directory.
rm -rf ${GH_PAGES_DIR}/api
cp -a ${API_DIR} ${GH_PAGES_DIR}
