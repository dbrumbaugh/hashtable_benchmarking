#!/bin/bash
echo "Running benchmarks"

for i in benchmarks/*_bench
do
    $i 2>&1 >> benchmarks/benchmarks.log
done

echo ""
