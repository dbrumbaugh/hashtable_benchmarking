#include <iostream>
#include <chrono>
#include "hashtable.hpp"

using namespace std;

uint32_t testhash0(uint32_t hval)
{
    hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
    hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
    hval = (hval >> 16) ^ hval;

    return hval;
}


uint32_t testhash1(uint32_t hval)
{
    hval = ((hval >> 14) ^ hval) * 0x29a9f2b;
    hval = ((hval >> 14) ^ hval) * 0x29a9f2b;
    hval = (hval >> 14) ^ hval;

    return hval;
}


HashTable<uint32_t, int> *initialize_table(size_t n)
{
    auto table = new HashTable<uint32_t, int>(&testhash0);
    for (size_t i=0; i<n; i++) {
        table->insert(i, rand());
    }

    return table;
}


int main(int argc, char **argv)
{
    if (argc < 2) {
        cerr << "useage: mixedops <write-mix>";
        exit(1);
    }

    double write_mix = stod(argv[1]);

    auto test = initialize_table(1000000);

    double experiment_total = 0;
    for (size_t i=0; i<1000; i++) {
        double write = (double) rand() / (double) RAND_MAX;
        uint32_t key = rand();
        int value = rand();


        if (write < write_mix) {
            auto start = std::chrono::high_resolution_clock::now();
            auto val = test->insert(key, value);
            auto stop = std::chrono::high_resolution_clock::now();

            experiment_total += chrono::duration_cast<chrono::nanoseconds>(stop - start).count();
        } else {
            auto start = std::chrono::high_resolution_clock::now();
            auto val = test->access(key);
            auto stop = std::chrono::high_resolution_clock::now();

            experiment_total += chrono::duration_cast<chrono::nanoseconds>(stop - start).count();
        }
    }

    delete test;

    //tp_total *= 1e-9;
    experiment_total *= 1e-9;

    cout << write_mix << ",";
    cout << (experiment_total / 1000) << "\n";
}
