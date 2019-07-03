#!/bin/bash

echo "gammaMin nSankoff nTNI Time" > $4

hostFileName=$1
ptreeFileName=$2
enumFileName=$3

gammaMax=$(./sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 | grep 'Infection' | cut -d: -f 2)
gammaMin=$(( $(wc -l "$hostFileName" | rev | cut -d' ' -f2 | rev) - 1 ))

numSankoff=$(./sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 | grep 'Sankoff solutions' | cut -d: -f 2 | sed -e 's/ //g')

echo "gammaMax is $gammaMax and gammaMin is $gammaMin"

for gamma in $(seq $gammaMin $gammaMax)
do
  dimacsFileName="dimacs.cnf"
  varListFileName="varList.out"

  dimacsFolderName="dimacs"

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

      echo "$gamma $numSankoff $numTNI $totaltime" >> $4
      echo "minimum satisfiable gamma is $gamma"
      break
  fi

  echo ""

done
