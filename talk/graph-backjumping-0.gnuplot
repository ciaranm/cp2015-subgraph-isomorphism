# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4.2in,2.6in font '\tiny'

set size square

set output "gen-graph-backjumping-0.tex"

set key outside right center height 1 width -2 spacing 1.2

set border 3

set xlabel "Runtime without Backjumping (ms)"
set ylabel "Runtime with Backjumping (ms)"
set logscale xy
set xtics nomirror
set ytics nomirror
set grid
set xrange [1:1e8]
set yrange [1:1e8]
set format x '$10^{%T}$'
set format y '$10^{%T}$'

plot \
    x w l lt 1 lc 0 notitle, \
    NaN w p ps 1.5 pt 6 lc "#9a3a06" ti "LV (sat)", \
    NaN w p ps 1.5 pt 7 lc "#9a3a06" ti "LV (unsat)", \
    NaN w p ps 1.5 pt 1 lc "#7d2239" ti "BVG / BVGm", \
    NaN w p ps 1.5 pt 2 lc "#005133" ti "M4D / M4Dr", \
    NaN w p ps 1.5 pt 10 lc "#52473b" ti "SF (sat)", \
    NaN w p ps 1.5 pt 11 lc "#52473b" ti "SF (unsat)", \
    NaN w p ps 1.5 pt 3 lc "#d278ab" ti "r", \
    NaN w p ps 1.5 pt 12 lc "#9ab9ad" ti "football", \
    NaN w p ps 1.5 pt 8 lc "#5b537d" ti "images (sat)", \
    NaN w p ps 1.5 pt 9 lc "#5b537d" ti "images (unsat)", \
    NaN w p ps 1.5 pt 4 lc "#003865" ti "meshes (sat)", \
    NaN w p ps 1.5 pt 5 lc "#003865" ti "meshes (unsat)"

