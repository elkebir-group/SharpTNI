#!/bin/bash

home_dir="/home/sashitt2/TNI/data/recomb/simulation/cases"

echo "m kappa case# gammaMin nSankoff nTNI Time" > total.time

for i in 50 #10 30 #5 20 50
do
  for j in 1 2 3
  do
    for k in 1 2 3 4 5
    do

      echo "m = $i and kappa = $j and k = $k"

      ptreeFileName="$home_dir/m${i}_kappa$j/ptree$k.out"
      hostFileName="$home_dir/m${i}_kappa$j/host$k.out"
      
      enumFileName="$home_dir/m${i}_kappa$j/enum$k.out"

      gammaMax=$(./sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 | grep 'Infection' | cut -d: -f 2)
      gammaMin=$(( $i - 1 ))

      numSankoff=$(./sankoff "$hostFileName" "$ptreeFileName" "$enumFileName" -e -l 0 | grep 'Sankoff solutions' | cut -d: -f 2 | sed -e 's/ //g')

      echo "gammaMax is $gammaMax and gammaMin is $gammaMin"

      for gamma in $(seq $gammaMin $gammaMax)
      do
        dimacsFileName="$home_dir/m${i}_kappa$j/dimacs$k.cnf"
        varListFileName="$home_dir/m${i}_kappa$j/varList$k.out"

        dimacsFolderName="$home_dir/m${i}_kappa$j/case$k"

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

            echo "$i $j $k $gamma $numSankoff $numTNI $totaltime" >> total.time
            echo "minimum satisfiable gamma is $gamma"
            break
        fi

        echo ""

      done
    done
  done
done
