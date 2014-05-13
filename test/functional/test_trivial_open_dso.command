TEMP_DIR=`mktemp -d -t test_trivial_open_dso-XXXXXXXXXX`
./pmdatrivial --export-all "$TEMP_DIR"
if [ $? -ne 0 ]; then exit; fi
dbpmda -e -n "$TEMP_DIR/root" <<EOF
open dso libpmda_trivial.so trivial_init
text trivial.time
close
EOF
rm -rf "$TEMP_DIR"
