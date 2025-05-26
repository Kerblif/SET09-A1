#ifndef STRING_GENERATOR_H
#define STRING_GENERATOR_H

#include <string>
#include <vector>
#include <random>

class StringGenerator {
public:
    enum ArrayType {
        RANDOM,
        SORTED,
        REVERSE_SORTED,
        ALMOST_SORTED,
        COMMON_PREFIX
    };

private:
    std::string allowedChars;
    std::mt19937 rng;
    std::uniform_int_distribution<int> charDist;
    std::uniform_int_distribution<int> lengthDist;

    std::string generateSingleRandomString(int length);
    std::vector<std::string> generateRandomArrayInternal(int size);


public:
    StringGenerator(unsigned int seed = std::random_device{}());

    std::vector<std::string> generateStringArray(ArrayType type, int size, int prefixLengthForCommon = 5, double almostSortedSwapPercentage = 0.05);

    std::vector<std::string> generateRandomArray(int size);

    std::vector<std::string> generateSortedArray(int size);

    std::vector<std::string> generateReverseSortedArray(int size);

    std::vector<std::string> generateAlmostSortedArray(int size, int swapCount);

    std::vector<std::string> generateCommonPrefixArray(int size, int prefixLength);

    void saveArrayToFile(const std::vector<std::string>& arr, const std::string& filename);

    std::vector<std::string> loadArrayFromFile(const std::string& filename);

    void prepareTestArrays(const std::string& baseDir, int maxSize = 3000, int step = 100);
    // --- Конец старых методов ---
};

#endif // STRING_GENERATOR_H