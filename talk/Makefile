all : tables graphs
	latexmk -pdf -pdflatex='pdflatex -interaction=nonstopmode %O %S' talk

TABLES =

GRAPHS = gen-graph-cumulative-0.tex gen-graph-cumulative-1.tex gen-graph-cumulative-2.tex \
	 gen-graph-cumulative-3.tex gen-graph-cumulative-4.tex gen-graph-cumulative-5.tex \
	 gen-graph-cumulative-6.tex gen-graph-cumulative-7.tex \
	 gen-graph-best-other-0.tex gen-graph-best-other-1.tex gen-graph-best-other-2.tex \
	 gen-graph-backjumping-0.tex gen-graph-backjumping-1.tex \
	 gen-graph-fad-0.tex gen-graph-fad-1.tex gen-graph-fad-2.tex \
	 gen-graph-speedup-0.tex gen-graph-speedup-1.tex

tables : $(TABLES)

graphs : $(GRAPHS)

gen-graph-%.tex : graph-%.gnuplot
	gnuplot $<

