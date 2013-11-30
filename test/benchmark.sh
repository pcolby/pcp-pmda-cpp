#               Copyright Paul Colby 2013.
# Distributed under the Boost Software License, Version 1.0.
#       (See accompanying file LICENSE.md or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

#
# Really basic PMDA benchmark script.
#

echo
if [ $# -ne 5 ]; then
  echo -e "Usage: `basename $0` pmda_name metric_name stime|utime period duration_seconds\n"
  echo 'Examples:'
  echo "  `basename $0` simple simple.now utime 0.5s 5"
  echo "  `basename $0` trivial trivial.time stime 0.001s 10"
  echo
  exit 1
fi

# Find the PMDA's PID.
PMDA_PID=`pgrep "pmda$1"`
if [ -z "$PMDA_PID" ]; then
  echo "No pmda$1 process"
  exit 2
fi

# Start pmie sampling the PMDA.
echo -n "Fetching $2 @ $4 via pmie"
echo "$2" | pmie -t "$4" &
PMIE_PID=$!
echo "[$PMIE_PID]..."
sleep 0.5 # Give pmie a little time to initialize.

# Watch the PMDA's CPU utilization.
echo -n "Fetching pmda$1[$PMDA_PID] $3 utilization over $5 seconds: "
UTIL=`pmval -f5 -s1 -t"$5" "proc.psinfo.$3" -i "$PMDA_PID" | tail -n1 | tr -d ' '`
echo "$UTIL%"

# Stop the pmie instance we created above.
kill -TERM "$PMIE_PID"
echo
