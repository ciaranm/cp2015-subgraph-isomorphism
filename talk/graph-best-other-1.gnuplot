# vim: set et ft=gnuplot sw=4 :

set terminal tikz color size 4.2in,2.6in font '\tiny'

set output "gen-graph-best-other-1.tex"

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
    "runtimes.data" u (($2 == 1 && $3 == 0 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 11 notitle, \
    "runtimes.data" u (($2 == 1 && $3 == 0 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 11 notitle, \
    "runtimes.data" u (($2 == 1 && $3 == 0 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 11 notitle, \
    "runtimes.data" u (($2 == 1 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 10 notitle, \
    "runtimes.data" u (($2 == 1 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 10 notitle, \
    "runtimes.data" u (($2 == 1 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 10 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 0 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 7 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 0 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 7 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 0 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 7 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 6 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 6 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 6 notitle, \
    "runtimes.data" u (($2 == 3 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 1 notitle, \
    "runtimes.data" u (($2 == 3 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 1 notitle, \
    "runtimes.data" u (($2 == 3 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 1 notitle, \
    "runtimes.data" u (($2 == 4 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 1 notitle, \
    "runtimes.data" u (($2 == 4 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 1 notitle, \
    "runtimes.data" u (($2 == 4 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 1 notitle, \
    "runtimes.data" u (($2 == 5 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 2 notitle, \
    "runtimes.data" u (($2 == 5 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 2 notitle, \
    "runtimes.data" u (($2 == 5 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 2 notitle, \
    "runtimes.data" u (($2 == 6 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 2 notitle, \
    "runtimes.data" u (($2 == 6 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 2 notitle, \
    "runtimes.data" u (($2 == 6 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 2 notitle, \
    "runtimes.data" u (($2 == 7 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 3 notitle, \
    "runtimes.data" u (($2 == 7 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 3 notitle, \
    "runtimes.data" u (($2 == 7 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 3 notitle, \
    "runtimes.data" u (($2 == 8 && $3 == 0 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 9 notitle, \
    "runtimes.data" u (($2 == 8 && $3 == 0 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 9 notitle, \
    "runtimes.data" u (($2 == 8 && $3 == 0 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 9 notitle, \
    "runtimes.data" u (($2 == 8 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 8 notitle, \
    "runtimes.data" u (($2 == 8 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 8 notitle, \
    "runtimes.data" u (($2 == 8 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 8 notitle, \
    "runtimes.data" u (($2 == 9 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 12 notitle, \
    "runtimes.data" u (($2 == 9 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 12 notitle, \
    "runtimes.data" u (($2 == 9 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 12 notitle, \
    "runtimes.data" u (($2 == 10 && $3 == 0 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 5 notitle, \
    "runtimes.data" u (($2 == 10 && $3 == 0 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 5 notitle, \
    "runtimes.data" u (($2 == 10 && $3 == 0 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 5 notitle, \
    "runtimes.data" u (($2 == 10 && $3 == 1 && $5 < $4 && $5 < $6 && $5 < 1e8)?$5:NaN):($7):(1) w p ps variable lc 4 pt 4 notitle, \
    "runtimes.data" u (($2 == 10 && $3 == 1 && $4 <= $6 && $4 <= $5 && $4 < 1e8)?$4:NaN):($7):(1) w p ps variable lc 8 pt 4 notitle, \
    "runtimes.data" u (($2 == 10 && $3 == 1 && $6 < $4 && $6 <= $5 && $6 < 1e8)?$6:NaN):($7):(1) w p ps variable lc 2 pt 4 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 0 && $6 >= 1e8 && $6 >= 1e8 && $5 >= 1e8)?1e8:NaN):($7):(1) w p ps variable lc 0 pt 7 notitle, \
    "runtimes.data" u (($2 == 2 && $3 == 1 && $6 >= 1e8 && $6 >= 1e8 && $5 >= 1e8)?1e8:NaN):($7):(1) w p ps variable lc 0 pt 6 notitle, \
    "runtimes.data" u (($2 == 7 && $3 == 1 && $6 >= 1e8 && $6 >= 1e8 && $5 >= 1e8)?1e8:NaN):($7):(1) w p ps variable lc 0 pt 3 notitle, \
    "runtimes.data" u (($2 >= 8 && $3 == 1 && $6 >= 1e8 && $6 >= 1e8 && $5 >= 1e8)?1e8:NaN):($7):(1) w p ps 10 lc 0 pt 1 notitle, \
    "runtimes.data" u (($3 == -1 && $4<1e8)?$4:NaN):($7):(1) w p ps 10 lc 0 pt 1 notitle, \
    "runtimes.data" u (($3 == -1 && $6<1e8)?$6:NaN):($7):(1) w p ps 10 lc 0 pt 1 notitle, \
    "runtimes.data" u (($3 == -1 && $5<1e8)?$5:NaN):($7):(1) w p ps 10 lc 0 pt 1 notitle, \
    "runtimes.data" u (($3 == -2 && $4<1e8)?$4:NaN):($7):(1) w p ps 10 lc 0 pt 2 notitle, \
    "runtimes.data" u (($3 == -2 && $6<1e8)?$6:NaN):($7):(1) w p ps 10 lc 0 pt 2 notitle, \
    "runtimes.data" u (($3 == -2 && $5<1e8)?$5:NaN):($7):(1) w p ps 10 lc 0 pt 2 notitle, \
    x w l lt 0 notitle, \
    NaN w l lw 10 lc 8 ti "LAD", \
    NaN w l lw 10 lc 2 ti "VF2", \
    NaN w l lw 10 lc 4 ti "SND", \
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

