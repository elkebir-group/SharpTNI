# SharpTNI - Transmission Network Inference under a Weak Bottleneck

SharpTNI is a problem which takes a timed phylogeny with leaf labeling
and host entry-exit times as input and finds counts and uniformly
samples the number of minimum transmission number host labelings. It
also counts and samples the solution space of transmission networks with
minimum transmission number and a fixed co-tranmsission number.

![Overview of SharpTNI problem](overview.png)
![equation](https://latex.codecogs.com/gif.latex?%5Ctext%7BThe%20evolutionary%20history%20of%20the%20pathogenic%20strains%20in%20an%20outbreak%20is%20described%20by%20a%20timed%20phylogeny%20%7D%20%24T%24%5Ctext%7B%2C%20assigning%20a%20time-stamp%20%7D%24%5Ctau%28v%29%24%5Ctext%7B%20to%20every%20vertex%20%7D%24v%24.%5Ctext%7B%20In%20addition%2C%20each%20leaf%20%7D%24v%24%5Ctext%7B%20is%20labeled%20by%20the%20host%20%7D%24%5Chat%7B%5Cell%7D%28v%29%24%5Ctext%7B%20where%20the%20corresponding%20strain%20was%20observed%20%28indicated%20by%20colors%29.%20Epidemiological%20data%20further%20constrain%20the%20entrance%20and%20removal%20time%20%7D%24%5B%5Ctaue%28s%29%2C%5Ctaur%28s%29%24%5Ctext%7B%20of%20each%20host%20%7D%24s%24%5Ctext%7B.%20In%20the%20TNI%20problem%2C%20we%20seek%20a%20host%20labeling%20%7D%24%5Cell%24%5Ctext%7B%20with%20minimum%20transmission%20number%20%7D%24%5Cmu%24%5Ctext%7B%20and%20subsequently%20smallest%20co-transmission%20number%20%7D%24%5Cgamma%24%5Ctext%7B.%20%28b%29%20Host%20labeling%20%7D%24%5Cell_b%24%5Ctext%7B%20with%20minimum%20transmission%20%7D%24%5Cmu%5E*%3D4%24%5Ctext%7B%20but%20not%20the%20smallest%20co-transmission%20number%20%7D%24%5Cgamma%3D4%24%5Ctext%7B%2C%20resulting%20in%20a%20complex%20transmission%20network%20%7D%24N_b%24%5Ctext%7B.%20%28c%29%20Host%20labeling%20%7D%24%5Cell_c%24%5Ctext%7B%20with%20minimum%20transmission%20%7D%24%5Cmu%5E*%3D4%24%5Ctext%7B%20and%20smallest%20co-transmission%20number%20%7D%24%5Cgamma%5E*%3D2%24%5Ctext%7B%2C%20resulting%20in%20a%20parsimonious%20transmission%20network%20%7D%24N_c%24.)


## Contents

  1. [Compilation instructions](#compilation)
     * [Dependencies](#dep)
     * [Compilation](#comp)
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

The SharpTNI input is text based. There are two input files, host file
and ptree file. Each line of the host file has exactly 3 entries separated by ' '.
The format of each line of the host file is '\<host name\> \<entry time\> \<removal time\>' in
each line. The number of lines in the host file is the number of sampled hosts.
A ptree file gives the timed phylogeny with the leaf labeling. Each line
of ptree file has exactly 4 entries separated by ' '. The
format for each line of the ptree file is '\<node name\> \<child1 name\> \<child2 name\> \<host label\>'. The number of lines in the ptree file is the number of nodes in the timed phylogeny. The nodes of the tree in the file must be in post-order (all nodes must be preceded by their children). For a leaf the <child name> must be '0'.

<a name="sankoff"></a>

###  Sankoff Labeling (`sankoff`)

	Usage:
	  ./sankoff [--help|-h|-help] [-b] [-c] [-e] [-l int] [-r int] [-t] [-u
	int]
	     <host> / <transmission_tree> <ptree> <output_ptree>
	Where:
	  --help|-h|-help
	     Print a short help message
	  -b
	     is the tree non binary (default: false)
	  -c
	     Find consensus Sankoff solution (deafault: false)
	  -e
	     Enumerate all the solutions (default: false)
	  -l int
	     Enumeration solution number limit (default: intMax)
	  -r int
	     Root label (default: 0)
	  -t
	     Transmission tree instead of host file
	  -u int
	     Number of unsampled hosts (default: 0)


An example execution:

    $ ./sankoff ../data/sample/sample_host.out ../data/sample/sample_ptree.out ../data/sample/sample_enum.out -u 1 -e -l 5

<a name="sample"></a>
### Sankoff Sampling (`sample_sankoff`)

	Usage:
	  ./sample_sankoff [--help|-h|-help] [-b] [-l int] [-r int] [-u int]
	<host>
	     <ptree> <output_prefix>
	Where:
	  --help|-h|-help
	     Print a short help message
	  -b
	     is the tree non binary (default: false)
	  -l int
	     Number of samples (default: 11000)
	  -r int
	     Root label (default: 0)
	  -u int
	     Number of unsampled hosts (default: 0)

An example execution:

    $ ./sample_sankoff -l 1 -u 1 ../data/sample/sample_host.out ../data/sample/sample_ptree.out ../data/sample/sample


<a name="clique"></a>

### Optimum Clique Partitioning (`gamma`)

	Usage:
	  ./gamma [--help|-h|-help] [-b] [-u int] <host> <ptree_sol>
	Where:
	  --help|-h|-help
	     Print a short help message
	  -b
	     is the tree non binary (default: false)
	  -u int
	     Number of unsampled hosts (default: 0)

An example execution:

    $ ./gamma -u 1 ../data/sample/sample_host.out ../data/sample/sample_idx0_count1.out 2> ../data/sample/example.dot
    $ dot -Tpng ../data/sample/example.dot -o ../data/sample/example.png

<a name="sat"></a>

### SAT formulation (`dimacs`)

	Usage:
	  ./dimacs [--help|-h|-help] [-k int] [-r int] [-t] [-u int]
	     <host> / <transmission_tree> <ptree> <output_dimacs_file>
	     <output_varlist_file>
	Where:
	  --help|-h|-help
	     Print a short help message
	  -k int
	     number of co-infection events (default: m-1)
	  -r int
	     Root label (default: 0)
	  -t
	     Transmission tree instead of host file
	  -u int
	     Number of unsampled hosts (default: 0)

An example execution:

    $ ./dimacs ../data/sample/sample_host.out ../data/sample/sample_ptree.out ../data/sample/sample_dimacs.cnf ../data/sample/sample_varlist.txt -u 1 -k 4
