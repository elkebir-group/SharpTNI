## Satisfiable:

```
$ python 3sat2tni.py 3 2 1 2 -3 -1 -2 -3 > sat_tree.txt 2>
sat_leaflabels.txt
```

Enumerating solutions:

```
$ ../../build/sankoff ../../script/sat_tree.txt ../../script/sat_leaflabels.txt sat.sol -r 7 -e -b
```

Grepping sat.sol, we find that smallest gamma is 10:

```
$ grep "#" sat.sol | cut -d'=' -f 4 | sort -u
 10
 11
 12
```

This is equal to `2*(VARS+CLAUSES)`, hence formula is satisfiable.

We find all solutions with minimum gamma as follows.

```
$ grep "gamma = 10" sat.sol
# idx = 11 -- root = 7 -- gamma = 10
# idx = 14 -- root = 7 -- gamma = 10
# idx = 17 -- root = 7 -- gamma = 10
# idx = 19 -- root = 7 -- gamma = 10
# idx = 28 -- root = 7 -- gamma = 10
# idx = 29 -- root = 7 -- gamma = 10
# idx = 34 -- root = 7 -- gamma = 10
# idx = 35 -- root = 7 -- gamma = 10
# idx = 39 -- root = 7 -- gamma = 10
# idx = 48 -- root = 7 -- gamma = 10
# idx = 50 -- root = 7 -- gamma = 10
# idx = 51 -- root = 7 -- gamma = 10
# idx = 53 -- root = 7 -- gamma = 10
# idx = 69 -- root = 7 -- gamma = 10
# idx = 70 -- root = 7 -- gamma = 10
# idx = 71 -- root = 7 -- gamma = 10
```

Next, we visualize solution 17 as follows.

```
$ ../../script/viz.py 3 2 sat.sol 17 > /tmp/G17.dot
$ dot -Tpng /tmp/G17.dot -o G17.png
```

![G17.png](G17.png)

## Unsatisfiable

```
python 3sat2tni.py 3 8 -1 -2 -3 -1 -2 3 -1 2 -3 1 -2 -3 1 2 -3 1 -2 3 -1 2 3 1 2 3 > unsat_tree.txt 2> unsat_leaflabels.txt
```

Enumerating solutions:

```
$ ./sankoff ../../script/unsat_tree.txt ../../script/unsat_leaflabels.txt sat.sol -r 7 -e -b
```

Grepping unsat.sol, we find that smallest gamma is 23:

```
$ grep "#" unsat.sol | cut -d'=' -f 4 | sort -u
 23
 24
 25
```

This is greater than `2*(VARS+CLAUSES)`, hence formula is unsatisfiable.
