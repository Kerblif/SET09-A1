#include <string>
#include <vector>

#include "sort.h"

using namespace std;

inline char charAtPos(const string& s, int d, int* comparisons = nullptr) {
    if (comparisons && d < s.length()) (*comparisons)++;
    return d < s.length() ? s[d] : 0;
}

int msdRadixSort(vector<string>& arr, int lo, int hi, int d, int comparisons) {
    if (hi <= lo) return comparisons;

    const int R = 256;

    vector<string> aux(hi - lo + 1);
    vector<int> count(R + 2, 0);

    for (int i = lo; i <= hi; i++) {
        count[charAtPos(arr[i], d, &comparisons) + 2]++;
    }

    for (int r = 0; r < R + 1; r++) {
        count[r + 1] += count[r];
    }

    for (int i = lo; i <= hi; i++) {
        aux[count[charAtPos(arr[i], d, &comparisons) + 1]++] = arr[i];
    }

    for (int i = lo; i <= hi; i++) {
        arr[i] = aux[i - lo];
    }

    for (int r = 0; r < R; r++) {
        comparisons = msdRadixSort(arr, lo + count[r], lo + count[r + 1] - 1, d + 1, comparisons);
    }
    
    return comparisons;
}

int stringRadixSort(vector<string>& arr) {
    int n = arr.size();
    if (n <= 1) return 0;

    int comparisons = 0;
    comparisons = msdRadixSort(arr, 0, n - 1, 0, comparisons);
    return comparisons;
}
