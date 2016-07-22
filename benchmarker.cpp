#include "benchmarker.h"
#include "matcher.h"
#include "metric.h"
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

void Benchmarker::individualRun(const Matcher& matcher, const Metric& metric,
        const vector<Point>& centers, double cutoff) {

    vector<vector<int>> plane;
    vector<int> quotas;

    clock_t beginInc = clock();
    matcher.incrementalMatching(centers, plane, quotas, cutoff);
    clock_t endInc = clock();
    double timeInc = elapsed(beginInc, endInc);
    if (cutoff == 0) {
        cout<<"cutoff: "<<cutoff<<"\tInc:"<<timeInc<<endl;
        is_stable(plane, centers, metric, 0.1);
        return;
    }

    vector<vector<int>> planeSH = plane;
    vector<int> quotasSH = quotas;
    vector<vector<int>> planeNN;
    vector<int> quotasNN;
    if (cutoff <= 0.01) {
        planeNN = plane;
        quotasNN = quotas;
    }

    clock_t beginBF = clock();
    matcher.allPairSortingMatching(centers, plane, quotas);
    clock_t endBF = clock();
    double timeBF = elapsed(beginBF, endBF);
    double totalTimeBF = timeInc+timeBF;

    double timeNN;
    double totalTimeNN;
    if (cutoff <= 0.01) {
        clock_t beginNN = clock();
        matcher.nearestNeighborMatching(centers, planeNN, quotasNN);
        clock_t endNN = clock();
        timeNN = elapsed(beginNN, endNN);
        totalTimeNN = timeInc+timeNN;
    } else {
        timeNN = -1;
        totalTimeNN = -1;
    }

    clock_t beginSH = clock();
    matcher.siteHeapMatching(centers, planeSH, quotasSH);
    clock_t endSH = clock();
    double timeSH = elapsed(beginSH, endSH);
    double totalTimeSH = timeInc+timeSH;

    cout<<"cutoff: "<<cutoff<<"\t";
    cout<<"BF:"<<timeBF<<" ("<<totalTimeBF<<")\t";
    cout<<"NN:"<<timeNN<<" ("<<totalTimeNN<<")\t";
    cout<<"SH:"<<timeSH<<" ("<<totalTimeSH<<")" << endl;

//    cout<<"Inc:"<<timeInc<<"\t BF:"<<totalTimeBF;
//    cout<<"Inc:"<<totalTimeBF<<":"<<totalTimeNN<<"\t";
//    cout<<timeInc<<":"<<timeBF<<":"<<timeNN<<"\t";

//    if (totalTimeBF > 0 and totalTimeNN > 0) {
//        int fracInc_BF = round(100*timeInc/totalTimeBF);
//        int fracBF_BF = round(100*timeBF/totalTimeBF);
//        int fracInc_NN = round(100*timeInc/totalTimeNN);
//        int fracNN_NN = round(100*timeNN/totalTimeNN);
//        cout<<fracInc_BF<<":"<<fracBF_BF<<"\t"<<fracInc_NN<<":"<<fracNN_NN<<endl;
//    }

//    bool stabBF = is_stable(plane, centers, metric, 0.1);
//    if (not stabBF) cout << "BF not stable" << endl;
//    if (cutoff <= 0.01) {
//        bool stabNN = is_stable(planeNN, centers, metric, 0.1);
//        if (not stabNN) cout << "NN not stable" << endl;
//    }
//    bool stabSH = is_stable(planeSH, centers, metric, 0.1);
//    if (not stabSH) cout << "SH not stable" << endl;

//    if (not stabBF or not stabNN or not stabSH) exit(0);
}

void Benchmarker::run() {
    qsrand(0);
    vector<Metric> metrics;
    metrics.push_back(Metric(2));
    metrics.push_back(Metric(1));
    metrics.push_back(Metric(Num::inf()));
    metrics.push_back(Metric(1.5));

//    vector<int> ns {30};
//    vector<int> ks {30};
//    vector<double> cutoffs {0.005};
    vector<int> ns {1000};
    vector<int> ks {1000, 10000};
//    vector<double> cutoffs {0, 0.005, 0.01};
    vector<double> cutoffs {0, 0.001, 0.0025, 0.005, 0.01, 0.015, 0.02, 0.025, 0.03, 0.035, 0.04, 0.045, 0.05};

    for (Metric metric : metrics) {
        for (int n : ns) {
            Matcher matcher(n, metric);
            for (int k : ks) {
                vector<Point> centers = randomCenters(n, k);
                cout<<"===== metric: "<<metric<<"\tn: "<<n<<"\tk: "<<k<<" ====="<<endl;
                for (double cutoff : cutoffs) {
                    individualRun(matcher, metric, centers, cutoff);
                }
            }
        }
    }
}
