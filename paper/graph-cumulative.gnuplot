# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4.5in,3.8in font '\scriptsize'

set output "gen-graph-cumulative.tex"

set multiplot

set border 3

# set key bottom right at screen 0.94, screen 0.1 Left vertical box height 1 width -3 spacing 1.2 maxrows 4 opaque
set nokey

set xlabel "Runtime (ms)"
set ylabel "Cumulative Number of Instances Solved"
set logscale x
set xtics nomirror
set ytics nomirror add (2487)
set grid
set xrange [1:1e8]
set yrange [0:2487]
set format x '$10^{%T}$'

set arrow from 2277e3, 2250 to screen 0.705, screen 0.57 lw 1 back filled

set arrow from 60e3,2250 to 86400e3,2250 front nohead
set arrow from 60e3,2487 to 86400e3,2487 front nohead
set arrow from 60e3,2250 to 60e3,2487 front nohead
set arrow from 86400e3,2250 to 86400e3,2487 front nohead

plot \
    "graph-cumulative.data" u ($7):($7 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "Our Parallel" lc 1, \
    "graph-cumulative.data" u ($4):($4 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "Our Sequential" lc 1 dt ".", \
    "graph-cumulative.data" u ($9):($9 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "SND" lc 5, \
    "graph-cumulative.data" u ($3):($3 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "LAD" lc 8, \
    "graph-cumulative.data" u ($8):($8 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "VF2" at end lc 2

set size 0.32, 0.40
set origin 0.545, 0.17
set bmargin 0; set tmargin 0; set lmargin 0; set rmargin 0
unset arrow
set border 15
clear

set nokey
set xrange [60e3:86400e3]
set xtics 0e3 add ("1m" 60e3) add ("1h" 3600e3) add ("1d" 86400e3) mirror
set ytics mirror
set mytics 5
set yrange [2250:2487]
set xlabel ""
set ylabel ""
set grid xtics ytics mytics

plot \
    "graph-cumulative.data" u ($7 >= 86400e3 ? 86400e3 : $7):($7 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{Our Parallel}' at end lc 1, \
    "graph-cumulative.data" u ($4 >= 86400e3 ? 86400e3 : $4):($4 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{Our Sequential}' at end lc 1 dt ".", \
    "graph-cumulative.data" u ($9 >= 86400e3 ? 86400e3 : $9):($9 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{SND}' at end lc 5, \
    "graph-cumulative.data" u ($3 >= 86400e3 ? 86400e3 : $3):($3 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{LAD}' at end lc 8

unset multiplot
