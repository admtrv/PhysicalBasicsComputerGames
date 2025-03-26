#!/bin/bash

DATA_FILE="../cmake-build-debug/task10/data.txt"
if [ ! -f "$DATA_FILE" ]; then
    echo "error: file $DATA_FILE not found"
    exit 1
fi

PLOT1=$(mktemp)
PLOT2=$(mktemp)
PLOT3=$(mktemp)
PLOT4=$(mktemp)

cat <<EOF > "$PLOT1"
set title "Local X vs Time"
set xlabel "Time (s)"
set ylabel "Local X Position (units)"
set grid
set key off
plot "$DATA_FILE" using 1:2 with lines lw 2 lc rgb "blue"
pause -1
EOF

cat <<EOF > "$PLOT2"
set title "Speed vs Time"
set xlabel "Time (s)"
set ylabel "Speed (units/s)"
set grid
set key off
plot "$DATA_FILE" using 1:3 with lines lw 2 lc rgb "green"
pause -1
EOF

cat <<EOF > "$PLOT3"
set title "Global X vs Time"
set xlabel "Time (s)"
set ylabel "X Position (units)"
set grid
set key off
plot "$DATA_FILE" using 1:4 with lines lw 2 lc rgb "red"
pause -1
EOF

cat <<EOF > "$PLOT4"
set title "Global Y vs Time"
set xlabel "Time (s)"
set ylabel "Y Position (units)"
set grid
set key off
plot "$DATA_FILE" using 1:5 with lines lw 2 lc rgb "purple"
pause -1
EOF

gnuplot -persist "$PLOT1" &
gnuplot -persist "$PLOT2" &
gnuplot -persist "$PLOT3" &
gnuplot -persist "$PLOT4" &

sleep 1
rm "$PLOT1" "$PLOT2" "$PLOT3" "$PLOT4"
