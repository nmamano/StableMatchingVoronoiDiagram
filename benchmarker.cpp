#include "benchmarker.h"
#include "matchingutils.h"
#include "fastmatcher.h"
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <QtWidgets>

using namespace std;

Benchmarker::Benchmarker()
{
}

double Benchmarker::elapsed(clock_t begin, clock_t end) {
    return double(end - begin)/CLOCKS_PER_SEC;
}

void Benchmarker::run() {
    int n = 100;
    int k = 500;
    qsrand(0);
    FastMatcher matcher(n);
    vector<Point> centers = randomCenters(n, k);

    const int cutoffs[10] = {0, 100, 1000, 10000, 100000, 500000, 1000000, 1500000, 2000000, 10000000};
    cout << "n = " << n << " k = " << k << " test" << endl;
    for (int cutoff : cutoffs) {
        matcher.setEarlyCutoff(cutoff);
        clock_t begin = clock();
        vector<vector<int>> plane =
                matcher.query(centers);
        clock_t end = clock();
        cout << "Cutoff: " << cutoff << " time: " << elapsed(begin, end) << endl;
        if (not is_stable(plane, centers)) {cout << "Error: result not stable" << endl; }
    }
}
