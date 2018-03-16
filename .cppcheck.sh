#!/bin/bash

EXTRA_ARGS=

# Cppcheck 1.65+ supports the --config-exclude option.
cppcheck --version | grep -iE '^cppcheck ([2-9]|1\.(6[5-9]|[7-9][0-9]|[1-9][0-9]{2}))'
if [ "$?" -eq 0 ]; then
  EXTRA_ARGS="$EXTRA_ARGS --config-exclude=/usr/include"
  EXTRA_ARGS="$EXTRA_ARGS -I/usr/include"
fi

RC=0
for platform in unix{32,64} win{32{A,W},64}; do
  echo "$platform:"
  cppcheck $EXTRA_ARGS --enable=all --error-exitcode=1 \
           -Iinclude --platform=$platform --std={c++03,c++11,posix} \
           --suppress={*:/usr/include/*,missingIncludeSystem} \
           --suppress=unusedStructMember:test/unit/src/fake_libpcp.cpp \
           "$@" ./include/pcp-cpp/*.hpp .
  RC=$((RC+$?))
done
exit $RC
