#!/bin/bash
exec_dir="../build"

echo "$5"

if [ $# != 6 -a $# != 7 ]
then
  echo "Usage: $0 <number of unsampled hosts> <number of uniform samples> <host file> <ptree file> <log file> <unigen output dir> (<exec dir>, default='../build')"
  exit 1
fi

if [ $# -gt 6 ]
then
  exec_dir=$7
fi

nh=$1
nsamples=$2
hostFileName=$3
ptreeFileName=$4
logFile=$5
dimacsFolderName=$6

echo "gammaMin nSankoff nTNI Time" > $logFile

if [ ! -e $exec_dir/sankoff ]
then
  echo "Missing executable $exec_dir/sankoff"
  exit 1
elif [ ! -e $exec_dir/dimacs ]
then
  echo "Missing executable $exec_dir/dimacs"
  exit 1
elif [ ! -e $exec_dir/approxmc ]
then
  echo "Mising executable $exec_dir/approxmc"
  exit 1
elif [ ! -e $exec_dir/unigen ]
then
  echo "Missing executable $exec_dir/unigen"
  exit 1
elif [ ! -f $exec_dir/UniGen2.py ];
then
  echo "Missing script $exec_dir/Unigen2.py"
  exit 1
fi

ln -s $exec_dir/unigen unigen

enumFileName="temp.out"

gammaMax=$( $exec_dir/sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 -u $nh | grep 'Infection' | cut -d: -f 2)
gammaMin=$(( $(wc -l "$hostFileName" | rev | cut -d' ' -f2 | rev) + $nh - 1 ))
#gammaMin=$(( $( $exec_dir/sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0  -u $nh | grep 'number of hosts' | cut -d' ' -f 5) - 1 ))

numSankoff=$( $exec_dir/sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 -u $nh | grep 'Sankoff solutions' | cut -d: -f 2 | sed -e 's/ //g')

rm -rf $enumFileName

echo "gammaMax is $gammaMax and gammaMin is $gammaMin"

for gamma in $(seq $gammaMin $gammaMax)
do
  dimacsFileName="dimacs.cnf"
  varListFileName="varList.out"

  echo "checking for $gamma"

  $exec_dir/dimacs "$hostFileName" "$ptreeFileName" "$dimacsFileName" "$varListFileName" -k $gamma &> /dev/null

  numStringTNI=$( $exec_dir/approxmc "$dimacsFileName"| grep 'Number of solutions' | cut -d: -f 2)
  preExponentTNI=$(echo "$numStringTNI" | cut -dx -f 1)
  numlogTNI=$(echo "$numStringTNI" | cut -d^ -f 2)

  numTNI=$(( $preExponentTNI* $(( 1 << $numlogTNI )) ))

  if [ $numTNI -gt 0 ]
  then
    if [ $numlogTNI -lt 7 ]
    then
        varcount=$(wc -l $varListFileName | cut -d' ' -f 1)

        for idx in $(seq 1 $(( 7-$numlogTNI )) )
        do
              # add dummy variable to dimacs file
            echo "$(( $varcount + $idx )) -$(( $varcount + $idx )) 0" >> $dimacsFileName
            echo "c ind $(( $varcount + $idx )) 0" >> $dimacsFileName
        done
    fi

      totaltime=$(python $exec_dir/UniGen2.py -samples=$nsamples "$dimacsFileName" "$dimacsFolderName" | grep 'Total time for all UniGen2 calls' $tmpFileName | cut -d' ' -f 7)

      echo "$gamma $numSankoff $numTNI $totaltime" >> $logFile
      echo "minimum satisfiable gamma is $gamma"
      break
  fi

  echo ""

done

rm -rf unigen
