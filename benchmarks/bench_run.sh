#!/bin/bash
echo "Running benchmarks"


echo "\tRunning baseline benchmark"
echo "n,throughput,latency" > baseline.log
for (( i = 1; i<1000000; i = i + 10000 )); do
    ./baseline_bench $i >> baseline.log
done
echo "\tFinished running baseline benchmark"

echo ""
