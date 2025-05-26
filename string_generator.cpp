#include "string_generator.h"
#include <fstream>
#include <algorithm>
#include <filesystem> // Для prepareTestArrays, если используется
#include <iostream>   // Для prepareTestArrays, если используется

StringGenerator::StringGenerator(unsigned int seed) {
    allowedChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-.";
    rng = std::mt19937(seed);
    charDist = std::uniform_int_distribution<int>(0, allowedChars.size() - 1);
    lengthDist = std::uniform_int_distribution<int>(10, 200);
}

std::string StringGenerator::generateSingleRandomString(int length) {
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; i++) {
        result.push_back(allowedChars[charDist(rng)]);
    }
    return result;
}

std::vector<std::string> StringGenerator::generateRandomArrayInternal(int size) {
    std::vector<std::string> result;
    result.reserve(size);
    for (int i = 0; i < size; i++) {
        int length = lengthDist(rng);
        result.push_back(generateSingleRandomString(length));
    }
    return result;
}

std::vector<std::string> StringGenerator::generateStringArray(ArrayType type, int size, int prefixLengthForCommon, double almostSortedSwapPercentage) {
    if (size == 0) return {};
    std::vector<std::string> arr;

    switch (type) {
        case RANDOM:
            arr = generateRandomArrayInternal(size);
            break;
        case SORTED:
            arr = generateRandomArrayInternal(size);
            std::sort(arr.begin(), arr.end());
            break;
        case REVERSE_SORTED:
            arr = generateRandomArrayInternal(size);
            std::sort(arr.begin(), arr.end(), std::greater<std::string>());
            break;
        case ALMOST_SORTED:
            arr = generateRandomArrayInternal(size);
            std::sort(arr.begin(), arr.end());
            if (size > 1) {
                int swaps = static_cast<int>(size * almostSortedSwapPercentage);
                if (swaps == 0 && size > 1) swaps = 1; // Ensure at least one swap for small arrays if percentage is low
                std::uniform_int_distribution<int> indexDist(0, size - 1);
                for (int i = 0; i < swaps; ++i) {
                    int idx1 = indexDist(rng);
                    int idx2 = indexDist(rng);
                    std::swap(arr[idx1], arr[idx2]);
                }
            }
            break;
        case COMMON_PREFIX:
            arr.reserve(size);
            if (prefixLengthForCommon <= 0 || prefixLengthForCommon > 200) prefixLengthForCommon = 5; // Default sensible prefix
            std::string prefix = generateSingleRandomString(prefixLengthForCommon);
            for (int i = 0; i < size; ++i) {
                int totalLength = lengthDist(rng);
                int suffixLength = totalLength - prefixLengthForCommon;
                if (suffixLength < 0) suffixLength = 0;
                if (prefixLengthForCommon + suffixLength < 10) {
                    suffixLength = 10 - prefixLengthForCommon;
                    if (suffixLength < 0) suffixLength = 0;
                }

                std::string suffix = generateSingleRandomString(suffixLength);
                arr.push_back(prefix + suffix);
            }
            break;
    }
    return arr;
}

std::vector<std::string> StringGenerator::generateRandomArray(int size) {
    return generateStringArray(RANDOM, size);
}

std::vector<std::string> StringGenerator::generateSortedArray(int size) {
    return generateStringArray(SORTED, size);
}

std::vector<std::string> StringGenerator::generateReverseSortedArray(int size) {
    return generateStringArray(REVERSE_SORTED, size);
}

std::vector<std::string> StringGenerator::generateAlmostSortedArray(int size, int swapCount) {
    double percentage = (size > 0) ? static_cast<double>(swapCount) / size : 0.05;
    return generateStringArray(ALMOST_SORTED, size, 5, percentage);
}

std::vector<std::string> StringGenerator::generateCommonPrefixArray(int size, int prefixLength) {
    return generateStringArray(COMMON_PREFIX, size, prefixLength);
}

void StringGenerator::saveArrayToFile(const std::vector<std::string> &arr, const std::string &filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    outFile << arr.size() << "\n";
    for (const auto &str : arr) {
        outFile << str << "\n";
    }
    outFile.close();
}

std::vector<std::string> StringGenerator::loadArrayFromFile(const std::string &filename) {
    std::ifstream inFile(filename);
    std::vector<std::string> result;
    if (!inFile.is_open()) {
        std::cerr << "Failed to open file for reading: " << filename << std::endl;
        return result;
    }
    int size;
    inFile >> size;
    inFile.ignore();
    result.reserve(size);
    std::string line;
    for (int i = 0; i < size; i++) {
        if (!std::getline(inFile, line)) break;
        result.push_back(line);
    }
    inFile.close();
    return result;
}

void StringGenerator::prepareTestArrays(const std::string &baseDir, int maxSize, int step) {
    std::filesystem::create_directories(baseDir + "/random");
    std::filesystem::create_directories(baseDir + "/sorted");
    std::filesystem::create_directories(baseDir + "/reverse");
    std::filesystem::create_directories(baseDir + "/almost");
    std::filesystem::create_directories(baseDir + "/prefix");

    for (int currentSize = step; currentSize <= maxSize; currentSize += step) {
        if (currentSize == 0 && step !=0) continue;

        std::cout << "Generating test data for size " << currentSize << "..." << std::endl;

        saveArrayToFile(generateStringArray(RANDOM, currentSize), baseDir + "/random/size_" + std::to_string(currentSize) + ".txt");
        saveArrayToFile(generateStringArray(SORTED, currentSize), baseDir + "/sorted/size_" + std::to_string(currentSize) + ".txt");
        saveArrayToFile(generateStringArray(REVERSE_SORTED, currentSize), baseDir + "/reverse/size_" + std::to_string(currentSize) + ".txt");
        saveArrayToFile(generateStringArray(ALMOST_SORTED, currentSize), baseDir + "/almost/size_" + std::to_string(currentSize) + ".txt");
        saveArrayToFile(generateStringArray(COMMON_PREFIX, currentSize), baseDir + "/prefix/size_" + std::to_string(currentSize) + ".txt");
         if (step == 0) break;
    }
    std::cout << "Test arrays preparation complete in " << baseDir << std::endl;
}

std::vector<std::string> generateRandomStrings(int count, int length, char charsetStart, char charsetEnd) {
    std::vector<std::string> result;
    result.reserve(count);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(charsetStart, charsetEnd);
    for (int i = 0; i < count; ++i) {
        std::string str;
        str.reserve(length);
        for (int j = 0; j < length; ++j) {
            str.push_back(static_cast<char>(dist(gen)));
        }
        result.push_back(str);
    }
    return result;
}

std::vector<std::string> generateRandomStringsVaryingLength(int count,
                                                            int minLength,
                                                            int maxLength,
                                                            char charsetStart,
                                                            char charsetEnd) {
    std::vector<std::string> result;
    result.reserve(count);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> charDist(charsetStart, charsetEnd);
    std::uniform_int_distribution<> lengthDist(minLength, maxLength);
    for (int i = 0; i < count; ++i) {
        int currentLength = lengthDist(gen);
        std::string str;
        str.reserve(currentLength);
        for (int j = 0; j < currentLength; ++j) {
            str.push_back(static_cast<char>(charDist(gen)));
        }
        result.push_back(str);
    }
    return result;
}
