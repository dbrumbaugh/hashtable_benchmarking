#include <iostream>
#include <chrono>
#include "hashtable.hpp"

using namespace std;


uint32_t testhash(uint32_t hval)
{
    hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
    hval = ((hval >> 16) ^ hval) * 0x45d9f3b;
    hval = (hval >> 16) ^ hval;

    return hval;
}


int main(int argc, char **argv)
{
    if (argc < 3) {
        cout << "Must provide an argument..";
        exit(1);
    }

    double n = stod(argv[1]);
    int m = stoi(argv[2]);
    auto test = new HashTable<uint32_t, int>(10000, testhash, 1, n);
    double total_time = 0.0;

    auto tp_start = std::chrono::high_resolution_clock::now();
    for (int i=0; i<m; i++) {
        auto lat_start = std::chrono::high_resolution_clock::now();
        test->insert(i, rand());
        auto lat_stop = std::chrono::high_resolution_clock::now();
        total_time += chrono::duration_cast<chrono::nanoseconds>(lat_stop - lat_start).count();
    }
    auto tp_stop = std::chrono::high_resolution_clock::now();

    double tp_total = std::chrono::duration_cast<chrono::nanoseconds>(tp_stop - tp_start).count();

    tp_total *= 1e-9;
    total_time *= 1e-9;

    cout << n << ",";
    cout << (m / tp_total) << ",";
    cout << (total_time / m) << "\n";
}

