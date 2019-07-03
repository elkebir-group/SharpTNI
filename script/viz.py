#!/usr/bin/python
import sys

if len(sys.argv) != 5:
    sys.stderr.write("Usage: %s <#VARS> <#CLAUSES> <SOL> <IDX>\n" % sys.argv[0])
    sys.exit(1)

n = int(sys.argv[1])
m = int(sys.argv[2])
sol = sys.argv[3]
idx = int(sys.argv[4])

with open(sol) as f:
    solution = f.readlines()

nrVertices = 1 + m + n + 3*m + 2*n

offset = idx * (nrVertices + 1) + 1

print "digraph G {"

for v in range(nrVertices):
    s = solution[offset + v].split()
    if int(s[-1]) == 2*n + 1:
        label = "_|_"
    else:
        label = (int(s[-1]) - 1)
        if label % 2 == 0:
            label = "x%d" % ((label / 2) + 1)
        else:
            label = "!x%d" % ((label / 2) + 1)

    print " ", v + 1, "[label=\"%d\\n%s\"]" % (v+1,label)

for v in range(nrVertices):
    s = solution[offset + v].split()
    for child in s[1:-1]:
        if child != '0':
            print v + 1, "->", child
print "}"
