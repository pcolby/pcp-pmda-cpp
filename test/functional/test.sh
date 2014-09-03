#!/bin/bash

DIFF=`which diff`

TEST_DIR=`dirname "$0"`

while IFS= read -d '' -r COMMAND; do
    TEST_NAME=`basename $COMMAND .command`
    echo -n "Checking $TEST_NAME: "
    . "$COMMAND" > "$TEST_NAME.output"; RC=$?
    if [ $RC -ne 0 ]; then exit $RC; fi
    if [ -e "$TEST_DIR/$TEST_NAME.expected" ]; then
        DIFF_OUTPUT=`"$DIFF" -u "$TEST_DIR/$TEST_NAME.expected" "$TEST_NAME.output"`; RC=$?
        if [[ $RC -ne 0 &&  -e "$TEST_DIR/$TEST_NAME.alternative" ]]; then
            DIFF_OUTPUT2=`"$DIFF" -u "$TEST_DIR/$TEST_NAME.alternative" "$TEST_NAME.output"`; RC=$?
        fi
        if [ $RC -ne 0 ]; then
            echo 'FAILED (files differ)' >&2
            echo "$DIFF_OUTPUT"
            if [ -n "$DIFF_OUTPUT2" ]; then echo "$DIFF_OUTPUT2"; fi
            exit $RC
        fi
    else
        echo -n '(no expectations) '
    fi
    echo "OK"
done < <(find "$TEST_DIR" -name 'test_*.command' -type f -print0)
