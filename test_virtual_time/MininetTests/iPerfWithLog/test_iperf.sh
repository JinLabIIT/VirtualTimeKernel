#!/bin/bash

rm *.log

SWs="2"
PerfTime=60 # seconds
BWs="100" # mb

Durations="1.0 0.1 0.01" # seconds
Intervals="0.5 0.1 0.05" # seconds

for bw in $BWs
do
        for dur in $Durations
        do
                for int in $Intervals
                do
                        echo "************************************************************"
                        echo "****** Freeze ${dur}s for every ${int}s with ${bw}mb link ******"
                        echo "************************************************************"
                        ./mn_iperf.py -n $SWs --perf_time $PerfTime --bw $bw \
                                --duration $dur --interval $int
                        cat Sw${SWs}Bw${bw}Frz${dur}Int${int}Bsl.log
                        cat Sw${SWs}Bw${bw}Frz${dur}Int${int}Vir.log
                done
        done
done
