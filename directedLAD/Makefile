CFLAGS = -O3 -Wall -fstrict-aliasing -std=c99

test:main
	@echo
	@echo "Test 1: Partial subgraph isomorphism for non labelled graphs"
	./main -p pattern.txt -t target.txt -v
	@echo
	@echo "Test 2: Partial subgraph isomorphism for labelled graphs"
	./main -p patternL.txt -t targetL.txt -v -l
	@echo
	@echo "Test 3: Induced subgraph isomorphism for non labelled graphs"
	./main -p pattern.txt -t target.txt -v -i
	@echo
	@echo "Test 4: Induced subgraph isomorphism for labelled graphs"
	./main -p patternL.txt -t targetL.txt -v -l -i

main:main.c allDiff.c graph.c lad.c domains.c
	$(CC) $(CFLAGS) main.c -o $@
