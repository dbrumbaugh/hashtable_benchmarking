#!/bin/ksh
echo "Running benchmarks"


echo -e "\tRunning baseline benchmark"
echo "n,throughput,latency" > baseline.log
for (( i = 1; i<1000000; i = $i + 10000 )); do
    ./benchmarks/baseline_bench $i >> baseline.log
done
echo -e "\tFinished running baseline benchmark"

echo ""
