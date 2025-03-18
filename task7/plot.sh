#!/bin/bash

DATA_FILE="../cmake-build-debug/task7/data.txt"
if [ ! -f "$DATA_FILE" ]; then
    echo "error: file $DATA_FILE not found"
    exit 1
fi

PLOT_SCRIPT_X=$(mktemp)
PLOT_SCRIPT_Y=$(mktemp)

cat <<EOF > "$PLOT_SCRIPT_X"
set title "X Position vs Time"
set xlabel "Time (s)"
set ylabel "X Position (units)"
set grid
set key left top
plot "$DATA_FILE" using 1:2 with lines title "Package x(t)", \
     "$DATA_FILE" using 1:4 with lines title "Ammo x(t)"
pause -1
EOF

cat <<EOF > "$PLOT_SCRIPT_Y"
set title "Y Position vs Time"
set xlabel "Time (s)"
set ylabel "Y Position (units)"
set grid
set key right top
plot "$DATA_FILE" using 1:3 with lines title "Package y(t)", \
     "$DATA_FILE" using 1:5 with lines title "Ammo y(t)"
pause -1
EOF

gnuplot -persist "$PLOT_SCRIPT_X" &
gnuplot -persist "$PLOT_SCRIPT_Y" &

sleep 1
rm "$PLOT_SCRIPT_X" "$PLOT_SCRIPT_Y"
