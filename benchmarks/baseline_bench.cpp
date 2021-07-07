#include <iostream>
#include <chrono>
#include "hashtable.hpp"

using namespace std;



int main(int argc, char **argv)
{
    if (argc < 2) {
        cout << "Must provide an argument..";
        exit(1);
    }

    int n = stoi(argv[1]);
    auto test = new HashTable<int, int>(10000);
    double total_time = 0.0;

    auto tp_start = std::chrono::high_resolution_clock::now();
    for (int i=0; i<n; i++) {
        auto lat_start = std::chrono::high_resolution_clock::now();
        test->insert(i, rand());
        auto lat_stop = std::chrono::high_resolution_clock::now();
        total_time += chrono::duration_cast<chrono::nanoseconds>(lat_stop - lat_start).count();
    }
    auto tp_stop = std::chrono::high_resolution_clock::now();

    double tp_total = std::chrono::duration_cast<chrono::nanoseconds>(tp_stop - tp_start).count();

    tp_total *= 1e-9;
    total_time *= 1e-9;

    cout << (n / tp_total) << "\n";
    cout << (total_time / n) << "\n";
}

