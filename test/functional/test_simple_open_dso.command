TEMP_DIR=`mktemp -d -t test_simple_open_dso-XXXXXXXXXX`
./pmdasimple --export-all "$TEMP_DIR"
if [ $? -ne 0 ]; then exit; fi
dbpmda -e -n "$TEMP_DIR/root" <<EOF
open dso libpmda_simple.so simple_init
close
EOF
rm -rf "$TEMP_DIR"
