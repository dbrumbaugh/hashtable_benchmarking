#!/bin/ksh
echo "Running benchmarks"

echo -e "\tRunning baseline benchmark"
echo "n,throughput,latency" > ./benchmarks/baseline.log
for (( i = 1; i<1000000; i = $i + 10000 )); do
#    ./benchmarks/baseline_bench $i >> ./benchmarks/baseline.log
done
echo -e "\tFinished running baseline benchmark"

echo -e "\tRunning mixedops benchmark"
echo "maxitr,write_mix,latency" > ./benchmarks/mixops.log
for (( i = 1; i<101; i = $i + 1 )); do
    ./benchmarks/mixedops_bench 10 $(( $i / 100.0  ))  >> ./benchmarks/mixops.log
done
echo -e "\tFinished running mixedops benchmark"
