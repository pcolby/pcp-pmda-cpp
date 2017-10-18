#!/bin/bash

EXTRA_ARGS=
HPP_FILES=./include/pcp-cpp/*.hpp

# Cppcheck 1.65+ supports the --config-exclude option.
cppcheck --version | grep -iE '^cppcheck ([2-9]|1\.(6[5-9]|[7-9][0-9]|[1-9][0-9]{2}))'
if [ "$?" -eq 0 ]; then
  EXTRA_ARGS="$EXTRA_ARGS --config-exclude=/usr/include"
  EXTRA_ARGS="$EXTRA_ARGS -I/usr/include"
fi

# For reasons unknown, Travis CI's current version of Cppcheck (1.61 at the time of
# writing) is painfully slow when processing *.hpp files. As it is, we don't need to
# include them explicitly since we have test (*.cpp) code that covers every single
# *.hpp file anyway - we just like to include them explicitly for completness.
# @todo Remove this once Travis CI's Cppcheck version no long exhibits this issue.
if [ -n "$TRAVIS" ]; then
  echo 'Skipping explicit *.hpp args for Travis CI'
  #HPP_FILES=
fi

RC=0
for platform in unix{32,64} win{32{A,W},64}; do
  echo "$platform:"
  cppcheck $EXTRA_ARGS --enable=all --error-exitcode=1 \
           -Iinclude --platform=$platform --std={c++03,c++11,posix} \
           --suppress={*:/usr/include/*,missingIncludeSystem} \
           "$@" $HPP_FILES .
  RC=$((RC+$?))
done
exit $RC
