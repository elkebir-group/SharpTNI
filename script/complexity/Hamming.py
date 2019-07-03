#!/usr/bin/python
import sys

def parse_ptree(filename):
    labeling = []
    with open(filename) as f:
        for line in f:
            s = line.rstrip("\n").split()
            if s[1] != '0' or s[2] != '0':
                labeling.append(int(s[-1]))
    return labeling

def parse_ptree_ext2(filename):
    labeling = []
    with open(filename) as f:
        for line in f:
            s = line.rstrip("\n").split()
            labeling.append(int(s[-1]))

    transmissions = set([])
    with open(filename) as f:
        for idx, line in enumerate(f):
            s = line.rstrip("\n").split()
            if s[1] != '0' or s[2] != '0':
                child1 = int(s[1]) - 1
                child2 = int(s[2]) - 1
                if labeling[idx] != labeling[child1]:
                    transmissions.add((idx, labeling[idx], child1, labeling[child1]))
                if labeling[idx] != labeling[child2]:
                    transmissions.add((idx, labeling[idx], child2, labeling[child2]))

    return transmissions

def parse_ptree_ext(filename):
    labeling = []
    with open(filename) as f:
        for line in f:
            s = line.rstrip("\n").split()
            labeling.append(int(s[-1]))

    transmissions = set([])
    with open(filename) as f:
        for idx, line in enumerate(f):
            s = line.rstrip("\n").split()
            if s[1] != '0' or s[2] != '0':
                child1 = int(s[1]) - 1
                child2 = int(s[2]) - 1
                if labeling[idx] != labeling[child1]:
                    transmissions.add((labeling[idx],labeling[child1]))
                if labeling[idx] != labeling[child2]:
                    transmissions.add((labeling[idx],labeling[child2]))

    return transmissions

def Hamming_dist(l1, l2):
    assert len(l1) == len(l2)
    return sum([ 0 if l1[i] == l2[i] else 1 for i in range(len(l1) )]) / float(len(l1))

def trans_dist(t1, t2):
    denom = len(t1.union(t2))
    numerator = len(t1.symmetric_difference(t2))
    return (float(numerator)/2)/float(len(t1))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        sys.stderr.write("Usage: %s <ptree1> <ptree2>\n" % sys.argv[0])
        sys.exit(1)

    l1 = parse_ptree(sys.argv[1])
    l2 = parse_ptree(sys.argv[2])

    print dist(l1, l2)
