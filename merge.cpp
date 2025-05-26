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

bool lcpCompare(const string& a, const string& b, int* comparisons = nullptr) {
    int commonPrefix = lcp(a, b, 0, comparisons);
    if (commonPrefix == min(a.length(), b.length())) {
        return a.length() < b.length();
    } else {
        if (comparisons) (*comparisons)++;
        return a[commonPrefix] < b[commonPrefix];
    }
}

int merge(vector<string>& arr, int left, int mid, int right, vector<string>& temp, int comparisons) {
    int i = left;
    int j = mid + 1;
    int k = 0;

    while (i <= mid && j <= right) {
        if (lcpCompare(arr[i], arr[j], &comparisons)) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = arr[i++];
    }

    while (j <= right) {
        temp[k++] = arr[j++];
    }

    for (i = 0; i < k; i++) {
        arr[left + i] = temp[i];
    }
    
    return comparisons;
}

int mergeSortHelper(vector<string>& arr, int left, int right, vector<string>& temp, int comparisons) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        comparisons = mergeSortHelper(arr, left, mid, temp, comparisons);
        comparisons = mergeSortHelper(arr, mid + 1, right, temp, comparisons);

        comparisons = merge(arr, left, mid, right, temp, comparisons);
    }
    return comparisons;
}

int stringMergeSort(vector<string>& arr) {
    int n = arr.size();
    if (n <= 1) return 0;

    int comparisons = 0;
    vector<string> temp(n);
    comparisons = mergeSortHelper(arr, 0, n - 1, temp, comparisons);
    return comparisons;
}
