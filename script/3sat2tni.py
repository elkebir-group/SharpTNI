#!/usr/bin/python
import sys

def lit2str(y):
    if y < 0:
        return "!x%d" % -y
    else:
        return "x%d" % y

if len(sys.argv) < 2:
    sys.stderr.write("Usage: %s <#VARS> <#CLAUSES> ...\n" % sys.argv[0])
    sys.exit(1)

n = int(sys.argv[1])
m = int(sys.argv[2])

phi = map(int, sys.argv[3:])

if len(phi) != 3*m:
    sys.stderr.write("Invalid formula\n")
    sys.exit(1)

for p in range(m):
    for i in range(3):
        if not(1 <= phi[3*p+i] <= n) and not(-n <= phi[3*p+i] <= -1):
            sys.stderr.write("Invalid clause %d and literal %d\n" % (p, phi[3*p+i]))


for i in range(1, n+1):
    sys.stderr.write("%s 0.5 1\n" % lit2str(i))
    sys.stderr.write("%s 0.5 1\n" % lit2str(-i))

sys.stderr.write("_|_ 0 0.1\n")

# leaves, variables
for i in range(1,n+1):
    print 1, 2*(i-1)+1
    print 1, 2*(i-1)+2

# leaves, clauses
delta = 0.1 / (m*2)
for p in range(m):
    offset = 0.6 + (p+1)*delta*2
    for i in range(3):
        literal = phi[3*p+i]
        if literal > 0:
            print offset, 2*(literal-1)+1
        else:
            print offset, 2*(-literal-1)+2

# interval vertices, variables
for i in range(1,n+1):
    print 0.5, -1, 2*(i-1)+1, 2*(i-1)+2

# interval vertices, variables
for p in range(m):
    offset = 0.6 + 1e-3 + (p)*delta*2
    print offset, -1, 2*n + p*3 + 1, 2*n + p*3 + 2, 2*n + p*3 + 3

# root
print 0, -1, " ".join(map(str, range(2*n+3*m+1, 2*n+3*m+1+m+n)))
