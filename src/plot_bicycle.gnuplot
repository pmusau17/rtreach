set terminal png font arial 16 size 500, 600
set output "reach_bicycle.png"

set title "Step=0.0022, Time= 7ms" font "arial,20"
set xlabel "x"
set ylabel "y"

# ranges for pendulum
set xrange [-1.1:0.3]
set yrange [0.0:1.0]

load "bicycle_initial.gnuplot.txt"
load "bicycle_intermediate.gnuplot.txt"
load "bicycle_final.gnuplot.txt"

plot \
   1/0 lw 4 lc rgb 'blue' with lines t 'Initial', \
   1/0 lw 4 lc rgb 'dark-green' with lines t 'Intermediate', \
   1/0 lw 4 lc rgb 'red' with lines t 'Final'
