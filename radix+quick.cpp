#include <string>
#include <vector>
#include <random>

#include "sort.h"

using namespace std;

inline char charAtPos(const string& s, int d, int* comparisons = nullptr) {
    if (comparisons && d < s.length()) (*comparisons)++;
    return d < s.length() ? s[d] : 0;
}

static int lcp(const string& a, const string& b, int start = 0, int* comparisons = nullptr) {
    int len = min(a.length(), b.length());
    int i;
    for (i = start; i < len; i++) {
        if (comparisons) (*comparisons)++;
        if (a[i] != b[i]) {
            break;
        }
    }
    return i;
}

static int stringQuickSortHelper(vector<string>& arr, int left, int right, int comparisons) {
    if (left >= right) return comparisons;

    mt19937 rng(random_device{}());
    uniform_int_distribution<int> dist(left, right);
    int pivotIndex = dist(rng);
    swap(arr[left], arr[pivotIndex]);

    string pivot = arr[left];

    int lt = left;
    int gt = right;
    int i = left + 1;

    while (i <= gt) {
        int cmp = 0;

        int commonPrefix = lcp(arr[i], pivot, 0, &comparisons);

        if (commonPrefix == min(arr[i].length(), pivot.length())) {
            cmp = (arr[i].length() < pivot.length()) ? -1 : (arr[i].length() > pivot.length() ? 1 : 0);
        } else {
            comparisons++;
            cmp = (arr[i][commonPrefix] < pivot[commonPrefix]) ? -1 : 1;
        }

        if (cmp < 0) {
            swap(arr[lt++], arr[i++]);
        } else if (cmp > 0) {
            swap(arr[i], arr[gt--]);
        } else {
            i++;
        }
    }

    comparisons = stringQuickSortHelper(arr, left, lt - 1, comparisons);
    comparisons = stringQuickSortHelper(arr, gt + 1, right, comparisons);
    
    return comparisons;
}

int msdRadixSortWithQuickSwitch(vector<string>& arr, int lo, int hi, int d, int comparisons) {
    if (hi <= lo) return comparisons;

    const int R = 256; // ASCII range

    // Switch to string quicksort for small subarrays
    if (hi - lo < R) {
        comparisons = stringQuickSortHelper(arr, lo, hi, comparisons);
        return comparisons;
    }

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
        comparisons = msdRadixSortWithQuickSwitch(arr, lo + count[r], lo + count[r + 1] - 1, d + 1, comparisons);
    }
    
    return comparisons;
}

int stringRadixSortWithQuickSwitch(vector<string>& arr) {
    int n = arr.size();
    if (n <= 1) return 0;

    int comparisons = 0;
    comparisons = msdRadixSortWithQuickSwitch(arr, 0, n - 1, 0, comparisons);
    return comparisons;
}
