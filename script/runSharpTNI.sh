#!/bin/bash
exec_dir="../build"

if [ ! -e $exec_dir/sankoff ]
then
  echo "Missing executable $exec_dir/sankoff"
  exit 1
fi

if [ "$3" != "" ]; then
  echo "gammaMin nSankoff nTNI Time" > $3
else
  echo "Usage: $0 <ARG1> <ARG2> ..."
  echo "not enough arguments"
  exit 0
fi


if [ "$1" != "" ]; then
  hostFileName=$1
else
  echo "not enough arguments"
  exit 0
fi

if [ "$2" != "" ]; then
  ptreeFileName=$2
else
  echo "not enough arguments"
  exit 0
fi

enumFileName="temp.out"

gammaMax=$(./sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 | grep 'Infection' | cut -d: -f 2)
gammaMin=$(( $(wc -l "$hostFileName" | rev | cut -d' ' -f2 | rev) - 1 ))

numSankoff=$(./sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 | grep 'Sankoff solutions' | cut -d: -f 2 | sed -e 's/ //g')

rm -rf $enumFileName

echo "gammaMax is $gammaMax and gammaMin is $gammaMin"

for gamma in $(seq $gammaMin $gammaMax)
do
  dimacsFileName="dimacs.cnf"
  varListFileName="varList.out"

  if [ "$4" != "" ]; then
    dimacsFolderName=$4
  else
	echo "not enough arguments"
	exit 0
  fi

  echo "checking for $gamma"

  ./dimacs "$hostFileName" "$ptreeFileName" "$dimacsFileName" "$varListFileName" -k $gamma &> /dev/null

  numStringTNI=$(./approxmc "$dimacsFileName"| grep 'Number of solutions' | cut -d: -f 2)
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

      totaltime=$(python UniGen2.py -samples=11000 "$dimacsFileName" "$dimacsFolderName" | grep 'Total time for all UniGen2 calls' $tmpFileName | cut -d' ' -f 7)

      echo "$gamma $numSankoff $numTNI $totaltime" >> $3
      echo "minimum satisfiable gamma is $gamma"
      break
  fi

  echo ""

done
