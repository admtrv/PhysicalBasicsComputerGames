#!/bin/bash

DATA_FILE="../cmake-build-debug/task5/data.txt"
if [ ! -f "$DATA_FILE" ]; then
    echo "error: file $DATA_FILE not found"
    exit 1
fi

PLOT_SCRIPT=$(mktemp)

cat <<EOF > "$PLOT_SCRIPT"
set title "Distance vs Time"
set xlabel "Time (s)"
set ylabel "Distance (units)"
set grid
plot "$DATA_FILE" using 1:2 with lines title "Car"
pause -1
EOF

gnuplot -persist "$PLOT_SCRIPT"

rm "$PLOT_SCRIPT"
