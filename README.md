# SharpTNI - Transmission Network Inference under a Weak Bottleneck

SharpTNI is a problem which takes a timed phylogeny with leaf labeling
and host entry-exit times as input and finds counts and uniformly
samples the number of minimum transmission number host labelings. It
also counts and samples the solution space of transmission networks with
minimum transmission number and a fixed co-tranmsission number.

## Contents

  1. [Compilation instructions](#compilation)
     * [Dependencies](#dep)
     * [Compilatilon] (#comp)
  2. [Usage instcructions](#usage)
     * [I/O formats](#io)
     * [Sankoff Labeling](#sankoff)
     * [Sample Sankoff](#sample)
     * [Optimum Clique Partitioning](#gamma)
     * [SAT Formulation](#dimacs)

<a name="compilation"></a>
## Compilation instructions

<a name="dep"></a>
### Dependencies

SharpTNI solver is written in C++11 and requires a modern C++ compiler
(GCC >= 4.8.1, or Clang). In addition it has the following dependencies

* [CMake](http://www.cmake.org/) (>=2.8)
* [Boost](http://www.boost.org) (>= 1.38)
* [LEMON](http://lemon.cs.elte.hu/trac/lemon) graph library (>= 1.3)

[Graphviz](http://www.graphviz.org) is required to visualize the resulting DOT files, but is not required for compilation.

<!--In case [doxygen](http://www.stack.nl/~dimitri/doxygen/) is available, extended source code documentation will be generated.-->

<a name="comp"></a>
### Compilation

To compile execute the following commands from the root of the
repository

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

The compilation results in the following files in the `build' directory

EXECUTABLE       | DESCRIPTION
-----------------|-------------
`sankoff`        | count/enumerate the minimum transmission number host labelings
`sample_sankoff` | uniformly sample minimum transmission number host labelings
`gamma`          | optimum clique partitioning for a given host labeling
`dimacs`         | SAT formulation for SharpTNI problem

<a name="usage"></a>
## Usage instructions

<a name="io"></a>
### I/O formats
