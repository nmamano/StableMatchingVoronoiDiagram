#include "benchmarker.h"
#include "diskgrower.h"
#include "metric.h"
#include "bigreedymatcher.h"
#include "allpairsorting.h"
#include "neighborheuristic.h"
#include "siteheap.h"
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

void Benchmarker::individualRun(const DiskGrower& matcher, const vector<BiGreedyMatcher*>& bimatchers,
        const Metric& metric, const vector<Point>& centers, double cutoff) {

    vector<vector<int>> plane;
    vector<int> quotas;
    matcher.initEmpty(plane, quotas, centers.size());

    int PIndex = 0;
    clock_t beginInc = clock();
    matcher.incrementalMatching(centers, plane, quotas, cutoff, PIndex);
    clock_t endInc = clock();
    double timeInc = elapsed(beginInc, endInc);

    if (cutoff == 0) {
        bool stable = is_stable(plane, centers, metric, 0.01, true);
        if (!stable) cout<<"INC not stable"<<endl;

        cout<<"INC"<<" "<<0<<" ("<<timeInc<<")\n";
        for (BiGreedyMatcher* bimatcher : bimatchers) {
            cout<<bimatcher->acronym()<<" "<<0<<" ("<<timeInc<<")\n";
        }
    } else {
        vector<vector<int>> planeCopy = plane;
        vector<int> quotasCopy = quotas;
        clock_t begin = clock();
        matcher.incrementalMatching(centers, planeCopy, quotasCopy, 0, PIndex);
        clock_t end = clock();
        bool stable = is_stable(planeCopy, centers, metric, 0.01, true);
        if (!stable) cout<<"INC2 not stable"<<endl;

        double time = elapsed(begin, end);
        cout<<"INC"<<" "<<time<<" ("<<timeInc+time<<")\n";

        for (BiGreedyMatcher* bimatcher : bimatchers) {
            planeCopy = plane;
            quotasCopy = quotas;
            bimatcher->setMetric(metric);
            begin = clock();
            bimatcher->solve(centers, planeCopy, quotasCopy);
            end = clock();
            cout << bimatcher->acronym() << " ";
            cout.flush();
            bool stable = is_stable(planeCopy, centers, metric, 0.01, true);
            if (not stable) {
                cout<<"not stable"<<endl;
            } else {
                time = elapsed(begin, end);
                cout<<time<<" ("<<timeInc+time<<")\n";
            }

        }
    }
    cout<<endl;
}

void Benchmarker::run() {
    qsrand(0);
    vector<Metric> metrics;
    metrics.push_back(Metric(2));
//    metrics.push_back(Metric(1));
    metrics.push_back(Metric(Num::inf()));
//    metrics.push_back(Metric(3));
    metrics.push_back(Metric(2.1));

    vector<BiGreedyMatcher*> bimatchers;
    bimatchers.push_back(new AllPairSorting());
    bimatchers.push_back(new SiteHeap());
    bimatchers.push_back(new NeighborHeuristic());

//    vector<int> ns {30};
//    vector<int> ks {30};
//    vector<double> cutoffs {0.005};
    vector<int> ns {1000};
//    vector<int> ks {100};
    vector<int> ks {1000, 10000};
    vector<double> cutoffs {0, 0.005, 0.01};
//    vector<double> cutoffs {0, 0.001, 0.0025, 0.005, 0.01, 0.015, 0.02, 0.025, 0.03, 0.035, 0.04, 0.045, 0.05};

    for (Metric metric : metrics) {
        for (int n : ns) {
            DiskGrower matcher(n, metric);
            for (int k : ks) {
                vector<Point> centers = intPoints(randomCenters(n, k, false));

                cout<<"===== metric: "<<metric<<"\tn: "<<n<<"\tk: "<<k<<" ====="<<endl;
                for (double cutoff : cutoffs) {
                    cout<<"*** cutoff: "<<cutoff<<"\n";
                    individualRun(matcher, bimatchers, metric, centers, cutoff);
                }
            }
        }
    }
}
