# SharpTNI script

Usage: ./runSharpTNI.sh \<number of unsampled hosts\> \<number of uniform samples\> \<host file\> \<ptree file\> \<log file\> \<unigen output dir\> (\<exec dir\>, default='../build')

    $ sh runSharpTNI.sh 4 11 ../data/sample/sample_host.out ../data/sample/sample_ptree.out ../data/sample/logFile.out ../data/sample/unigenOutput

To run the script the following executables and scripts must be in the exec directory (default: ../build)
  1. sankoff
  2. dimacs
  3. approxmc
  4. unigen
  5. UniGen2.py
