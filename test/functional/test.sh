#!/bin/bash

DIFF=`which diff`

TEST_DIR=`dirname "$0"`

if [ $# -lt 1 ]; then
    echo "Usage: `basename $0 .sh` <cmake-binary-dir>" 2>&1
    exit 1
fi

while IFS= read -d '' -r COMMAND; do
    TEST_NAME=`basename $COMMAND .command`
    echo -n "Checking $TEST_NAME: "
    . "$COMMAND" > "$TEST_NAME.output"; RC=$?
    if [ $RC -ne 0 ]; then exit $RC; fi
    if [ -e "$TEST_DIR/$TEST_NAME.expected" ]; then
        DIFF_OUTPUT=`"$DIFF" -u "$TEST_DIR/$TEST_NAME.expected" "$TEST_NAME.output"`; RC=$?
        if [ $RC -ne 0 ]; then
           echo 'FAILED (files differ)' >&2
            echo "\"$DIFF\" \"$TEST_DIR/$TEST_NAME.expected\" \"$TEST_NAME.output\""
            echo "$DIFF_OUTPUT"
            exit $RC
        fi
    fi
    echo "OK"
done < <(find "$TEST_DIR" -name 'test_*.command' -type f -print0)
