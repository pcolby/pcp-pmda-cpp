#!/bin/bash
RC=0
for platform in unix{32,64} win{32{A,W},64}; do
  echo "$platform:"
  cppcheck --enable=all --error-exitcode=1 \
           -I{/usr/,}include --platform=$platform --std={c++03,c++11,posix} \
           --suppress={*:/usr/include/*,missingIncludeSystem} --quiet \
           ./include/pcp-cpp/*.hpp .
  RC=$((RC+$?))
done
exit $RC
