#include <iostream>
#include <vector>
#include <string>

#include "string_sort_tester.h"

int main() {
    std::cout << "==== String Sorting Algorithm Benchmark using StringSortTester ====" << std::endl;
    std::cout << "Each test will be run multiple times to get an accurate average." << std::endl;
    std::cout << "=================================================================\n" << std::endl;

    StringSortTester tester;

    std::vector<int> dataSizes;
    for (int size = 100; size <= 3000; size += 100) {
        dataSizes.push_back(size);
    }

    int numRunsPerTest = 5;

    tester.runExperiments(dataSizes, numRunsPerTest);

    tester.printResultsSummary();
    tester.saveResultsToCsv("sort_performance.csv");

    std::cout << "\nBenchmark finished. Raw data saved to sort_performance.csv" << std::endl;
    std::cout << "You can use plot_results.py to visualize the data." << std::endl;

    return 0;
}
