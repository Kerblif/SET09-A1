#include <string>
#include <vector>
#include <random>

#include "sort.h"

using namespace std;

const int ASCII_CHARACTER_RANGE = 256;
const int MSD_TO_QUICK_SORT_THRESHOLD = 74;

inline char charAtPos(const string &s, int d, int *comparisons = nullptr) {
    if (d < s.length()) {
        return s[d];
    }
    return 0;
}

static int lcp_for_quick(const string &a, const string &b, int start, int *comparisons) {
    int len_a = a.length();
    int len_b = b.length();
    int min_len = std::min(len_a, len_b);
    int i;
    for (i = start; i < min_len; i++) {
        if (comparisons) (*comparisons)++;
        if (a[i] != b[i]) {
            break;
        }
    }
    return i - start;
}

static int stringQuickSortHelperForRadixSwitch(vector<string> &arr, int left, int right, int d, int comparisons) {
    if (left >= right) return comparisons;

    if (left < right) {
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<int> dist(left, right);
        int pivotIndex = dist(rng);
        std::swap(arr[left], arr[pivotIndex]);
    }

    string pivot_str = arr[left];
    char pivot_char_at_d = charAtPos(pivot_str, d, nullptr);

    int lt = left;
    int gt = right;
    int i = left + 1;

    while (i <= gt) {
        char current_char_at_d = charAtPos(arr[i], d, nullptr);
        if (comparisons) (comparisons)++;

        if (current_char_at_d < pivot_char_at_d) {
            std::swap(arr[lt++], arr[i++]);
        } else if (current_char_at_d > pivot_char_at_d) {
            std::swap(arr[i], arr[gt--]);
        } else {
            i++;
        }
    }

    comparisons = stringQuickSortHelperForRadixSwitch(arr, left, lt - 1, d, comparisons);
    if (pivot_char_at_d != 0) {
        comparisons = stringQuickSortHelperForRadixSwitch(arr, lt, gt, d + 1, comparisons);
    }
    comparisons = stringQuickSortHelperForRadixSwitch(arr, gt + 1, right, d, comparisons);

    return comparisons;
}

int msdRadixSortWithQuickSwitchRecursive(vector<string> &arr, int lo, int hi, int d, int comparisons) {
    if (hi <= lo) return comparisons;

    if ((hi - lo + 1) < MSD_TO_QUICK_SORT_THRESHOLD) {
        comparisons = stringQuickSortHelperForRadixSwitch(arr, lo, hi, d, comparisons);
        return comparisons;
    }

    vector<int> count(ASCII_CHARACTER_RANGE + 2, 0);
    vector<string> aux(hi - lo + 1);

    for (int i = lo; i <= hi; i++) {
        count[static_cast<unsigned char>(charAtPos(arr[i], d, nullptr)) + 2]++;
    }

    for (int r = 0; r < ASCII_CHARACTER_RANGE + 1; r++) {
        count[r + 1] += count[r];
    }

    for (int i = lo; i <= hi; i++) {
        aux[count[static_cast<unsigned char>(charAtPos(arr[i], d, nullptr)) + 1]++] = arr[i];
    }

    for (int i = lo; i <= hi; i++) {
        arr[i] = aux[i - lo];
    }

    for (int r = 0; r < ASCII_CHARACTER_RANGE; r++) {
        comparisons = msdRadixSortWithQuickSwitchRecursive(arr,
                                                           lo + count[r],
                                                           lo + count[r + 1] - 1,
                                                           d + 1,
                                                           comparisons);
    }

    return comparisons;
}

int stringRadixSortWithQuickSwitch(vector<string> &arr) {
    int n = arr.size();
    if (n <= 1) return 0;

    int comparisons = 0;
    comparisons = msdRadixSortWithQuickSwitchRecursive(arr, 0, n - 1, 0, comparisons);
    return comparisons;
}
