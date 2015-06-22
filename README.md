This repository contains the code, data and experimental scripts needed to
reproduce our subgraph isomorphism paper.

Parts of this repository are from other authors:

 * The vflib/vflib-2.0.6/ directory is the VF Library by the MIVIA Lab Research
   Group of the University of Salerno. More information can be found at:

    http://mivia.unisa.it/datasets/graph-database/vflib/

 * The directedLAD/ directory is Christine Solnon's LAD solver, version 2,
   which is distributed under the CeCILL-B FREE SOFTWARE LICENSE:

    http://liris.cnrs.fr/csolnon/LAD.html

 * The snd/ directory contains the Abscon solver. The txt files in the
   directory give its licence conditions.

 * The instances/ directory contains a subset of Christine Solnon's benchmark
   instances:

    http://liris.cnrs.fr/csolnon/SIP.html

Steps to reproduce the paper:

 * Build our code: run 'make' inside the code/ directory, after reading
   code/README.md for details on compiler requirements and dependencies.

 * Build LAD: run 'make' inside the directedLAD/ directory.

 * Build the vflib library: run 'make' inside the vflib/vflib-2.0.6/ directory.

 * Build the vflib solver: run 'make' inside the vflib/ directory.

 * Run the experiments: run 'make TIMEOUT=100000' inside the experiments/
   directory. (This will take a loooooong time. If a timeout is not specified,
   100s is used instead, which should finish within a few days.)

 * Copy experiments/results/*.data into the paper/ directory, to replace our
   experimental results with your own.

 * Run 'make' inside the paper/ directory. You will need Gnuplot 5 built with
   Lua support, and a full TeX Live installation.

