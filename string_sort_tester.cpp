#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <numeric>

#include "string_sort_tester.h"
#include "sort.h"

StringSortTester::StringSortTester() : generator(std::random_device{}()) {
    addAlgorithm("Merge Sort", stringMergeSort);
    addAlgorithm("Quick Sort", stringQuickSort);
    addAlgorithm("Radix Sort", stringRadixSort);
    addAlgorithm("Radix+Quick Sort", stringRadixSortWithQuickSwitch);

    addDataType("Random", StringGenerator::RANDOM);
    addDataType("Sorted", StringGenerator::SORTED);
    addDataType("Reverse Sorted", StringGenerator::REVERSE_SORTED);
    addDataType("Almost Sorted", StringGenerator::ALMOST_SORTED);
    addDataType("Common Prefix", StringGenerator::COMMON_PREFIX);
}

void StringSortTester::addAlgorithm(const std::string &name, SortFunction func) {
    algorithmsToTest.push_back({name, func});
}

void StringSortTester::addDataType(const std::string &name, StringGenerator::ArrayType type) {
    dataTypesToTest.push_back({name, type});
}

bool StringSortTester::verifySorted(const std::vector<std::string> &original,
                                    const std::vector<std::string> &sorted_arr) {
    if (original.size() != sorted_arr.size()) return false;
    if (original.empty()) return true;

    std::vector<std::string> correctly_sorted_original = original;
    std::sort(correctly_sorted_original.begin(), correctly_sorted_original.end());

    return sorted_arr == correctly_sorted_original;
}

void StringSortTester::runExperiments(const std::vector<int> &dataSizes, int numRunsPerTest) {
    results.clear();

    for (int size : dataSizes) {
        std::cout << "Testing with data size: " << size << std::endl;
        for (const auto &dataTypePair : dataTypesToTest) {
            const std::string &dataTypeName = dataTypePair.first;
            StringGenerator::ArrayType arrayType = dataTypePair.second;

            std::cout << "  Data Type: " << dataTypeName << std::endl;

            for (const auto &algoPair : algorithmsToTest) {
                const std::string &algoName = algoPair.first;
                SortFunction sortFunc = algoPair.second;

                std::cout << "    Algorithm: " << algoName << "..." << std::flush;

                std::vector<double> runTimesMs;
                std::vector<long long> runComparisons;
                bool allRunsVerified = true;

                for (int run = 0; run < numRunsPerTest; ++run) {
                    std::vector<std::string> currentArray = generator.generateStringArray(arrayType, size);
                    std::vector<std::string> originalForVerify = currentArray; // Copy for verification

                    auto startTime = std::chrono::high_resolution_clock::now();
                    long long comparisons = sortFunc(currentArray);
                    auto endTime = std::chrono::high_resolution_clock::now();

                    runTimesMs.push_back(
                        std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count() / 1000.0);
                    runComparisons.push_back(comparisons);

                    if (!verifySorted(originalForVerify, currentArray)) {
                        allRunsVerified = false;
                    }
                }

                double avgTimeMs = std::accumulate(runTimesMs.begin(), runTimesMs.end(), 0.0) / runTimesMs.size();
                long long avgComparisons = static_cast<long long>(std::accumulate(
                    runComparisons.begin(),
                    runComparisons.end(),
                    0LL) / runComparisons.size());

                results.push_back({algoName, dataTypeName, size, avgTimeMs, avgComparisons, allRunsVerified});
                std::cout << " Avg Time: " << std::fixed << std::setprecision(3) << avgTimeMs << "ms"
                        << ", Avg Comp: " << avgComparisons
                        << (allRunsVerified ? "" : " (VERIFICATION FAILED!)") << std::endl;
            }
            std::cout << "  -------------------------------------" << std::endl;
        }
        std::cout << "==========================================" << std::endl << std::endl;
    }
}

const std::vector<StringSortTester::ExperimentResult> &StringSortTester::getResults() const {
    return results;
}

void StringSortTester::saveResultsToCsv(const std::string &filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }

    outFile << "Algorithm,DataType,DataSize,Time_ms,Comparisons,Verified\n";
    for (const auto &res : results) {
        outFile << res.algorithmName << ","
                << res.arrayTypeName << ","
                << res.arraySize << ","
                << std::fixed << std::setprecision(6) << res.timeTakenMs << ","
                << res.comparisons << ","
                << (res.verified ? "true" : "false") << "\n";
    }
    outFile.close();
    std::cout << "Results saved to " << filename << std::endl;
}

void StringSortTester::printResultsSummary() const {
    std::cout << "\n--- Results Summary ---" << std::endl;
    for (const auto &res : results) {
        std::cout << "Algo: " << std::setw(18) << std::left << res.algorithmName
                << " | Type: " << std::setw(15) << std::left << res.arrayTypeName
                << " | Size: " << std::setw(5) << std::right << res.arraySize
                << " | Time: " << std::setw(10) << std::fixed << std::setprecision(3) << std::right << res.timeTakenMs
                << " ms"
                << " | Comp: " << std::setw(12) << std::right << res.comparisons
                << " | Verified: " << (res.verified ? "Yes" : "NO!") << std::endl;
    }
    std::cout << "-----------------------" << std::endl;
}
