# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4.2in,2.6in font '\tiny'

set output "gen-graph-best-other-0.tex"

set size square

set key outside right center height 1 width -2 samplen 1.5

set border 3

set xlabel "Virtual best other solver runtime (ms)"
set ylabel "Our sequential runtime (ms)"
set logscale xy
set xtics nomirror
set ytics nomirror
set grid
set xrange [1:1e8]
set yrange [1:1e8]
set format x '$10^{%T}$'
set format y '$10^{%T}$'

plot \
    x w l lt 0 notitle, \
    NaN w l lw 10 lc "#9a3a06" ti "LAD", \
    NaN w l lw 10 lc "#005133" ti "VF2", \
    NaN w l lw 10 lc "#d278ab" ti "SND", \
    NaN w p ps 0 lc "white" ti "~~~~~~~~~~~~~~", \
    NaN w p ps 1.5 pt 6 lc 8 ti "LV (sat)", \
    NaN w p ps 1.5 pt 7 lc 8 ti "LV (unsat)", \
    NaN w p ps 1.5 pt 1 lc 8 ti "BVG / BVGm", \
    NaN w p ps 1.5 pt 2 lc 8 ti "M4D / M4Dr", \
    NaN w p ps 1.5 pt 10 lc 8 ti "SF (sat)", \
    NaN w p ps 1.5 pt 11 lc 8 ti "SF (unsat)", \
    NaN w p ps 1.5 pt 3 lc 8 ti "r", \
    NaN w p ps 1.5 pt 12 lc 8 ti "football", \
    NaN w p ps 1.5 pt 8 lc 8 ti "images (sat)", \
    NaN w p ps 1.5 pt 9 lc 8 ti "images (unsat)", \
    NaN w p ps 1.5 pt 4 lc 8 ti "meshes (sat)", \
    NaN w p ps 1.5 pt 5 lc 8 ti "meshes (unsat)"

