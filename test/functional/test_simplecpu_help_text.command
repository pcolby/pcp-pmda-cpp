TEMP_DIR=`mktemp -d -t test_simplecpu_help_text-XXXXXXXXXX`
./pmdasimplecpu --export-all "$TEMP_DIR"
if [ $? -ne 0 ]; then exit; fi
dbpmda -e -n "$TEMP_DIR/root" <<EOF
open pipe pmdasimplecpu
text simplecpu.ticks.total
text simplecpu.ticks.cpu0
close
EOF
rm -rf "$TEMP_DIR"
