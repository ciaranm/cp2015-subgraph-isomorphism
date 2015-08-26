# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 3.5in,2.6in font '\tiny'

set output "gen-graph-cumulative-2.tex"

set border 3
set key off

set xlabel "Runtime (ms)"
set ylabel "Cumulative Number of Instances Solved"
set logscale x
set xtics nomirror
set ytics nomirror add (2487)
set grid
set xrange [1:1e8]
set yrange [0:2487]
set format x '$10^{%T}$'

plot \
    "runtimes.data" u ($6):($6 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "VF2" lc "#005133" lw 2, \
    "runtimes.data" u ($4):($4 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "LAD" at end lc "#9a3a06" lw 2

