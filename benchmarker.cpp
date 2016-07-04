#include "benchmarker.h"
#include "matcher.h"
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
    qsrand(0);
    vector<int> metrics {1, 2, -1};
    vector<int> ns {1000};
    vector<int> ks {1000, 5000, 10000};
    vector<int> cutoffs {0, -1};

    for (int n : ns) {
        Matcher matcher(n);
        for (int metric : metrics) {
            matcher.setMetric(metric);
            for (int k : ks) {
                vector<Point> centers = randomCenters(n, k);
                for (int cutoff : cutoffs) {
                    matcher.setCutoff(cutoff);
                    clock_t begin = clock();
                    vector<vector<int>> plane =
                            matcher.query(centers);
                    clock_t end = clock();
                    cout<<"n: "<<n<<" metric: "<<metric<<" k: "<<k<<" cutoff: "<<cutoff<<" time: "<<elapsed(begin, end)<<endl;
                    is_stable(plane, centers, metric);
                }
            }
        }
    }
}
