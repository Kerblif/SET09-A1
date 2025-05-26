#include <random>

#include "sort.h"

using namespace std;

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

int stringQuickSortHelper(vector<string>& arr, int left, int right, int comparisons) {
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

int stringQuickSort(vector<string>& arr) {
    int n = arr.size();
    if (n <= 1) return 0;

    int comparisons = 0;
    comparisons = stringQuickSortHelper(arr, 0, n - 1, comparisons);
    return comparisons;
}
