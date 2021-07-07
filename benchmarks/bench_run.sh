#!/bin/ksh
echo "Running benchmarks"


echo -e "\tRunning baseline benchmark"
echo "n,throughput,latency" > baseline.log
for (( i = 1; i<1000000; i = $i + 10000 )); do
    ./benchmarks/baseline_bench $i >> baseline.log
done
echo -e "\tFinished running baseline benchmark"

echo -e "\tRunning stride benchmark"
echo "stride,throughput,latency" > stride.log
for (( i = 1; i<100; i = $i + 1 )); do
    ./benchmarks/stride_bench $i 100000 >> stride.log
done
echo -e "\tFinished running stride benchmark"


echo -e "\tRunning lf benchmark"
echo "lf,throughput,latency" > load.log
for (( i = 50; i<91; i = $i + 1 )); do
    lf=$(( $i / 100.0 ))
    ./benchmarks/load_bench $lf 10000 >> load.log
done
echo -e "\tFinished running lf benchmark"

echo ""
