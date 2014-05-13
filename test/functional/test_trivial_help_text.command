TEMP_DIR=`mktemp -d -t test_trivial_text-XXXXXXXXXX`
./pmdatrivial --export-all "$TEMP_DIR"
if [ $? -ne 0 ]; then exit; fi
dbpmda -e -n "$TEMP_DIR/root" <<EOF
open pipe pmdatrivial
text trivial.time
close
EOF
rm -rf "$TEMP_DIR"
