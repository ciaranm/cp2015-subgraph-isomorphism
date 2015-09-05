# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4in,3in font '\tiny'

set output "gen-graph-cumulative-7.tex"

set multiplot

set border 3 lc "#c0000000"

# set key bottom right at screen 0.94, screen 0.1 Left vertical box height 1 width -3 spacing 1.2 maxrows 4 opaque
set nokey

set xlabel "Runtime (ms)" tc "#c0000000"
set ylabel "Cumulative Number of Instances Solved" tc "#c0000000"
set logscale x
set xtics nomirror
set ytics nomirror add (2487)
set grid
set xrange [1:1e8]
set yrange [0:2487]
set format x '$10^{%T}$'

set arrow from 2277e3, 2250 to screen 0.685, screen 0.65 lw 1 back filled

set arrow from 60e3,2250 to 86400e3,2250 front nohead
set arrow from 60e3,2487 to 86400e3,2487 front nohead
set arrow from 60e3,2250 to 60e3,2487 front nohead
set arrow from 86400e3,2250 to 86400e3,2487 front nohead

plot \
    "runtimes.data" u ($12):($12 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "Parallel" lc "#c0003865" lw 2 dt ".", \
    "runtimes.data" u ($7):($7 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "Glasgow" lc "#c0003865" lw 2, \
    "runtimes.data" u ($5):($5 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "SND" lc "#c0d278ab" lw 2, \
    "runtimes.data" u ($4):($4 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "LAD" lc "#c09a3a06" lw 2, \
    "runtimes.data" u ($6):($6 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "VF2" at end lc "#c0005133" lw 2

set size 0.32, 0.40
set origin 0.525, 0.25
set bmargin 0; set tmargin 0; set lmargin 0; set rmargin 0
unset arrow
set border 15 lc "#000000"
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
    "runtimes.data" u ($12 >= 86400e3 ? 86400e3 : $12):($12 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{1mm}{Parallel}' at end lc "#003865" lw 2 dt ".", \
    "runtimes.data" u ($7 >= 86400e3 ? 86400e3 : $7):($7 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{Glasgow}' at end lc "#003865" lw 2, \
    "runtimes.data" u ($5 >= 86400e3 ? 86400e3 : $5):($5 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{SND}' at end lc "#d278ab" lw 2, \
    "runtimes.data" u ($4 >= 86400e3 ? 86400e3 : $4):($4 >= 86400e3 ? 1e-10 : 1) smooth cumulative ti '\raisebox{0mm}{LAD}' at end lc "#9a3a06" lw 2

unset multiplot
