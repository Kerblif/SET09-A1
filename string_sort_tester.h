#ifndef STRING_SORT_TESTER_H
#define STRING_SORT_TESTER_H

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include "string_generator.h"

class StringSortTester {
    public:
        using SortFunction = std::function<int(std::vector<std::string>&)>;

        struct ExperimentResult {
            std::string algorithmName;
            std::string arrayTypeName;
            int arraySize;
            double timeTakenMs;
            long long comparisons;
            bool verified;
        };

    private:
        StringGenerator generator;
        std::vector<ExperimentResult> results;

        std::vector<std::pair<std::string, SortFunction>> algorithmsToTest;
        std::vector<std::pair<std::string, StringGenerator::ArrayType>> dataTypesToTest;

        bool verifySorted(const std::vector<std::string>& original, const std::vector<std::string>& sorted);


    public:
        StringSortTester();

        void addAlgorithm(const std::string& name, SortFunction func);
        void addDataType(const std::string& name, StringGenerator::ArrayType type);

        void runExperiments(const std::vector<int>& dataSizes, int numRunsPerTest = 5);

        const std::vector<ExperimentResult>& getResults() const;
        void saveResultsToCsv(const std::string& filename) const;
        void printResultsSummary() const;
};

#endif // STRING_SORT_TESTER_H