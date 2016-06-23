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
    vector<int> ns {1000};
    vector<int> ks {1000, 5000, 10000};
    qsrand(0);

    vector<int> cutoffs {0, -1};
    for (int i = 0; i < (int)ns.size(); i++) {
        int n = ns[i];
        FastMatcher matcher(n);
        for (int k : ks) {
            vector<Point> centers = randomCenters(n, k);
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
    }
}
