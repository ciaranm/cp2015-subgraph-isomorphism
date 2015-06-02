all : tables graphs
	latexmk -pdf -pdflatex='pdflatex -interaction=nonstopmode %O %S' paper

TABLES =

GRAPHS = gen-graph-backjumping.tex gen-graph-cumulative.tex gen-graph-fad.tex gen-graph-best-other.tex \
	 gen-graph-speedup.tex

tables : $(TABLES)

graphs : $(GRAPHS)

gen-graph-%.tex : graph-%.gnuplot
	gnuplot $<

