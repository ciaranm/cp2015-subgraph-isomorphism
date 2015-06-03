To build, type 'make'. You will need a C++14 compiler (we use GCC 4.9) and
Boost (built with threads enabled).

To run:

    ./solve_subgraph_isomorphism algorithm-name pattern-file target-file

You may need to increase the stack space, for larger graphs. In bash this is
done as follows:

    ulimit -s 1048576

The algorithm variations are:

    sequential
    threaded
    no-backjumping
    no-supplementals
    no-all-diff
    full-all-diff

The pattern and target files should be in the format used by LAD (see the
instances/ directory).

The output is like this:

    true|false search-nodes
    (p1 -> t1) (p2 -> t2) ... (pn -> tn)
    total-runtime thread-runtimes...

This code has been ripped out of a larger project, which includes other
variations, support for different file formats, various other algorithms, etc.
If you're looking for the most up-to-date code, rather than to reproduce the
paper, you may prefer this:

    https://github.com/ciaranm/parasols
