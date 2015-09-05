# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4in,3in font '\tiny'

set output "gen-graph-cumulative-4.tex"

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

plot \
    "runtimes.data" u ($7):($7 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "Glasgow" at end lc "#003865" lw 2, \
    "runtimes.data" u ($5):($5 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "SND" lc "#d278ab" lw 2, \
    "runtimes.data" u ($4):($4 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "LAD" lc "#9a3a06" lw 2, \
    "runtimes.data" u ($6):($6 >= 1e8 ? 1e-10 : 1) smooth cumulative ti "VF2" lc "#005133" lw 2

