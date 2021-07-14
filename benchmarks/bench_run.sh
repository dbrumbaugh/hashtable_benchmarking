#!/bin/ksh
echo "Running benchmarks"

echo -e "\tRunning baseline benchmark"
echo "n,throughput,latency" > ./benchmarks/baseline.log
for (( i = 1; i<1000000; i = $i + 10000 )); do
    ./benchmarks/baseline_bench $i >> ./benchmarks/baseline.log
done
echo -e "\tFinished running baseline benchmark"

echo -e "\tRunning stride benchmark"
echo "stride,throughput,latency" > ./benchmarks/stride.log
for (( i = 1; i<100; i = $i + 1 )); do
    ./benchmarks/stride_bench $i 100000 >> ./benchmarks/stride.log
done
echo -e "\tFinished running stride benchmark"


echo -e "\tRunning lf benchmark"
echo "lf,throughput,latency" > ./benchmarks/load.log
for (( i = 50; i<91; i = $i + 1 )); do
    lf=$(( $i / 100.0 ))
    ./benchmarks/load_bench $lf 10000 >> ./benchmarks/load.log
done
echo -e "\tFinished running lf benchmark"

echo -e "\tRunning mixedops benchmark"
echo "write_mix,latency" > ./benchmarks/mixops.log
for (( i = 1; i<101; i = $i + 1 )); do
    ./benchmarks/mixedops_bench $(( $i / 100.0  ))  >> ./benchmarks/mixops.log
done
echo -e "\tFinished running mixedops benchmark"
