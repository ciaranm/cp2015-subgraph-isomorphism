# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4.8in,6.4in font '\scriptsize'

set size square

set output "gen-graph-fad.tex"

set multiplot layout 2,1

set key outside right center height 1 width -2 spacing 1.2

set border 3

set xlabel "Recursive calls with counting-based all-different"
set ylabel "Recursive calls with matching-based all-different"
set logscale xy
set xtics nomirror
set ytics nomirror
set grid
set xrange [1:1e9]
set yrange [1:1e9]
set grid xtics ytics
set format x '$10^{%T}$'
set format y '$10^{%T}$'

plot \
    "graph-fad.data" u (($2 == 0 && $3 == 10) ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 5 lc 2 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 10) ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 4 lc 2 notitle, \
    "graph-fad.data" u (($2 == 0 && $3 == 2)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 7 lc 1 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 2)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 6 lc 1 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 3)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 1 lc 3 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 4)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 1 lc 3 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 5)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 2 lc 5 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 6)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 2 lc 5 notitle, \
    "graph-fad.data" u (($2 == 0 && $3 == 1)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 11 lc 7 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 1)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 10 lc 7 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 7)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 3 lc 8 notitle, \
    "graph-fad.data" u (($2 == 0 && $3 == 8)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 9 lc 6 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 8)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 8 lc 6 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 9)  ? ($5<1?1:($5>1e9?1e9:$5)) : NaN):($6<1?1:($6>1e9?1e9:$6)):(1) ps variable pt 12 lc 4 notitle, \
    x w l lt 1 lc 0 notitle, \
    NaN w p ps 0 lc "white" ti "~~~~~~~~~~~~~~", \
    NaN w p ps 1.5 pt 6 lc 1 ti "LV (sat)", \
    NaN w p ps 1.5 pt 7 lc 1 ti "LV (unsat)", \
    NaN w p ps 1.5 pt 1 lc 3 ti "BVG / BVGm", \
    NaN w p ps 1.5 pt 2 lc 5 ti "M4D / M4Dr", \
    NaN w p ps 1.5 pt 10 lc 7 ti "SF (sat)", \
    NaN w p ps 1.5 pt 11 lc 7 ti "SF (unsat)", \
    NaN w p ps 1.5 pt 3 lc 8 ti "r", \
    NaN w p ps 1.5 pt 12 lc 4 ti "football", \
    NaN w p ps 1.5 pt 8 lc 6 ti "images (sat)", \
    NaN w p ps 1.5 pt 9 lc 6 ti "images (unsat)", \
    NaN w p ps 1.5 pt 4 lc 2 ti "meshes (sat)", \
    NaN w p ps 1.5 pt 5 lc 2 ti "meshes (unsat)", \
    NaN w p ps 0 lc "white" ti "~~~~~~~~~~~~~~"

set xlabel "Recursive calls with value-deleting all-different"
set ylabel "Recursive calls with counting-based all-different"

plot \
    "graph-fad.data" u (($2 == 0 && $3 == 10) ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 5 lc 2 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 10) ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 4 lc 2 notitle, \
    "graph-fad.data" u (($2 == 0 && $3 == 2)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 7 lc 1 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 2)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 6 lc 1 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 3)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 1 lc 3 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 4)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 1 lc 3 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 5)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 2 lc 5 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 6)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 2 lc 5 notitle, \
    "graph-fad.data" u (($2 == 0 && $3 == 1)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 11 lc 7 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 1)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 10 lc 7 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 7)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 3 lc 8 notitle, \
    "graph-fad.data" u (($2 == 0 && $3 == 8)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 9 lc 6 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 8)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 8 lc 6 notitle, \
    "graph-fad.data" u (($2 == 1 && $3 == 9)  ? ($7<1?1:($7>1e9?1e9:$7)) : NaN):($5<1?1:($5>1e9?1e9:$5)):(1) ps variable pt 12 lc 4 notitle, \
    x w l lt 1 lc 0 notitle, \
    NaN w p ps 0 lc "white" ti "~~~~~~~~~~~~~~"

