#include "benchmarker.h"
#include "circlegrower.h"
#include "metric.h"
#include "bigreedymatcher.h"
#include "pairsort.h"
#include "pairheap.h"
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <QtWidgets>

using namespace std;

void Benchmarker::runExperiments()
{
    QTime now = QTime::currentTime();
    qsrand(now.msec());
    //    Benchmarker::cutoffExperiment(false, Num(2), true);
    //    Benchmarker::cutoffExperiment(true, Num(2), true);
    //    Benchmarker::cutoffExperiment(false, Num(1), true);
    //    Benchmarker::cutoffExperiment(true, Num(1), true);
    //    Benchmarker::cutoffExperiment(false, Num::inf(), true);
    //    Benchmarker::cutoffExperiment(true, Num::inf(), true);
//        Benchmarker::profileExperiment(false, Num(2), true);
//        Benchmarker::profileExperiment(true, Num(2), true);
//        Benchmarker::profileExperiment(false, Num(1), true);
//        Benchmarker::profileExperiment(true, Num(1), true);
//        Benchmarker::profileExperiment(false, Num::inf(), true);
//        Benchmarker::profileExperiment(true, Num::inf(), true);
//        Benchmarker::runtimeExperiment(false, Num(2), false);
//        Benchmarker::runtimeExperiment(true, Num(2), false);
//        Benchmarker::runtimeExperiment(false, Num(1), false);
//        Benchmarker::runtimeExperiment(true, Num(1), false);
//        Benchmarker::runtimeExperiment(false, Num::inf(), false);
//        Benchmarker::runtimeExperiment(true, Num::inf(), false);
//    Benchmarker::centroidExperiment(false, Num(2), false);
    Benchmarker::centroidExperiment(false, Num(2), true);
        exit(0);
}

void Benchmarker::compAlgorithms(bool real) {
    vector<Metric> metrics = getMetrics("2");
    vector<BiGreedyMatcher*> bimatchers = getMatchers("PS SH");
    vector<int> ns = getInts("1000");
    vector<int> ks = getInts("100 1000 10000");
    vector<double> cutoffs = getDoubles("0.01");

    qsrand(0);

    if (real) cout<<"real centers"<<endl;
    else cout<<"int centers"<<endl;

    for (Metric metric : metrics) {
        for (int n : ns) {
            CircleGrower matcher(n, metric);
            for (int k : ks) {
                cout<<"===== metric: "<<metric<<"\tn: "<<n<<"\tk: "<<k<<" ====="<<endl;
                vector<Point> intCenters;
                vector<DPoint> realCenters;
                if (real) realCenters = doublePoints(randomCenters(n, k, true));
                else intCenters = intPoints(randomCenters(n, k, false));
                for (double cutoff : cutoffs) {
                    cout<<"*** cutoff: "<<cutoff<<"\n";
                    if (real) individualRunReal(n, matcher, bimatchers, metric, realCenters, cutoff);
                    else individualRunInt(n, matcher, bimatchers, metric, intCenters, cutoff);
                }
            }
        }
    }
}

void Benchmarker::cutoffExperiment(bool real, Num metr, bool checkStable) {
    int n = 1000;
    vector<int> ks = getInts("100 1000 10000");
    vector<double> cutoffs = getDoubles("0 0.05 0.15 0.25 0.5 0.75 1 1.25 1.5 1.75 2 2.25 2.5 2.75 3");
    Metric metric(metr);
    int numRuns = 1;

    if (real) cout<<"real centers, metric "<<metr<<endl;
    else cout<<"int centers, metric "<<metr<<endl;

    clock_t initPlane1 = clock();
    CircleGrower circleGrower(n, metric);
    clock_t initPlane2 = clock();
    BiGreedyMatcher* bimatcher = getMatchers("PHLL")[0];
    bimatcher->setMetric(metric);

    cout<<"cutoff, k=100, k=1000, k=10000"<<endl;
    for (double cutoff : cutoffs) {
        cout<<cutoff;
        for (int k : ks) {
            double avg = 0;
            for (int numRun = 0; numRun < numRuns; numRun++) {
                vector<Point> intCenters;
                vector<DPoint> realCenters;
                if (real) realCenters = doublePoints(randomCenters(n, k, true));
                else intCenters = intPoints(randomCenters(n, k, false));
                Matching M(n, k);
                int PIndex = 0;
                clock_t circleGrower1 = clock();
                if (real) circleGrower.solveReal(realCenters, M, cutoff, PIndex);
                else circleGrower.solveInt(intCenters, M, cutoff, PIndex);
                clock_t circleGrower2 = clock();
                clock_t bimatcher1 = clock();
                if (real) bimatcher->solveReal(realCenters, M);
                else bimatcher->solveInt(intCenters, M);
                clock_t bimatcher2 = clock();
                double totalTime = elapsed(initPlane1, initPlane2) +
                                   elapsed(circleGrower1, circleGrower2) +
                                   elapsed(bimatcher1, bimatcher2);
                avg += totalTime;
                if (checkStable) {
                    bool stable;
                    if (real) stable = M.isStable(numPoints(realCenters), metric, 0.001, true);
                    else stable = M.isStable(numPoints(intCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
            }
            avg /= numRuns;
            cout<<", "<<avg;
        }
        cout<<endl;
    }
}

void Benchmarker::profileExperiment(bool real, Num metr, bool checkStable) {
    int n = 1000;
    int k = 10000;
    Metric metric(metr);
    int numRuns = 10;
    vector<double> cutoffs = getDoubles("0 0.05 0.15 0.25 0.5 0.75 1 1.25 1.5 1.75 2 2.25 2.5 2.75 3");
    if (real) cout<<"real centers, metric "<<metr<<endl;
    else cout<<"int centers, metric "<<metr<<endl;
    BiGreedyMatcher* bimatcher = getMatchers("PHLL")[0];
    bimatcher->setMetric(metric);

    cout<<"cutoff, CG, PHLL, total"<<endl;
    for (double cutoff : cutoffs) {
        cout<<cutoff;
        double avgCG = 0;
        double avgPHLL = 0;
        for (int numRun = 0; numRun < numRuns; numRun++) {
            vector<Point> intCenters;
            vector<DPoint> realCenters;
            if (real) realCenters = doublePoints(randomCenters(n, k, true));
            else intCenters = intPoints(randomCenters(n, k, false));
            Matching M(n, k);
            int PIndex = 0;
            clock_t circleGrower1 = clock();
            CircleGrower circleGrower(n, metric);
            if (real) circleGrower.solveReal(realCenters, M, cutoff, PIndex);
            else circleGrower.solveInt(intCenters, M, cutoff, PIndex);
            clock_t circleGrower2 = clock();
            clock_t bimatcher1 = clock();
            if (real) bimatcher->solveReal(realCenters, M);
            else bimatcher->solveInt(intCenters, M);
            clock_t bimatcher2 = clock();
            avgCG += elapsed(circleGrower1, circleGrower2);
            avgPHLL += elapsed(bimatcher1, bimatcher2);
            if (checkStable) {
                bool stable;
                if (real) stable = M.isStable(numPoints(realCenters), metric, 0.001, true);
                else stable = M.isStable(numPoints(intCenters), metric, 0.001, true);
                if (!stable) {
                    cout<<"not stable"<<endl;
                    exit(0);
                }
            }
        }
        avgCG /= numRuns;
        avgPHLL /= numRuns;
        cout<<", "<<avgCG<<", "<<avgPHLL<<", "<<avgCG+avgPHLL<<endl;
    }
}

void Benchmarker::centroidExperiment(bool real, Num metr, bool checkStable) {
    int n = 300;
    int k = 300;
    int KMeansIter = 100;
    Metric metric(metr);
    double cutoff = 0.10;
    int numRuns = 10;
    CircleGrower CG(n, metric);
    PairHeap PHLL(true, false);
    PHLL.setMetric(metric);

    if (real) cout<<"real centers, metric "<<metr<<endl;
    else cout<<"int centers, metric "<<metr<<endl;

    cout << "n = " << n << ", k = " << k << endl;
    cout << "optimal = " << CG.idealAvgDistPointCenter(n*n/k) << endl;

    {
        double avg = 0;
        int sampleSize = 100;
        for (int i = 0; i < sampleSize; i++) {
            if (real) {
                vector<DPoint> realCenters = doublePoints(randomCenters(n, k, true));
                int PIndex = 0;
                Matching M(n, k);
                CG.solveReal(realCenters, M, cutoff, PIndex);
                PHLL.solveReal(realCenters, M);
                if (checkStable) {
                    bool stable = M.isStable(numPoints(realCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
                avg += avgDistPointCenter(M.plane, numPoints(realCenters), metric);
            } else {
                vector<Point> intCenters = intPoints(randomCenters(n, k, false));
                int PIndex = 0;
                Matching M(n, k);
                CG.solveInt(intCenters, M, cutoff, PIndex);
                PHLL.solveInt(intCenters, M);
                if (checkStable) {
                    bool stable = M.isStable(numPoints(intCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
                avg += avgDistPointCenter(M.plane, numPoints(intCenters), metric);
            }
        }
        avg /= sampleSize;
        cout << "random = " << avg << endl;
    }

    cout << "p,\tdist" << endl;
    double p = -3;
    for (int i = 0; i <= 6*10; i++) {
        double avg = 0;
        for (int numRun = 0; numRun < numRuns; numRun++) {
            if (real) {
                vector<DPoint> realCenters = doublePoints(randomCenters(n, k, true));
                int PIndex = 0;
                Matching M(n, k);
                CG.solveReal(realCenters, M, cutoff, PIndex);
                PHLL.solveReal(realCenters, M);
                for (int iter = 0; iter < KMeansIter; iter++) {
                    realCenters = doublePoints(weightedCentroids(M.plane, numPoints(realCenters), metric, p, true));
                    PIndex = 0;
                    M = Matching(n, k);
                    CG.solveReal(realCenters, M, cutoff, PIndex);
                    PHLL.solveReal(realCenters, M);
                }
                if (checkStable) {
                    bool stable = M.isStable(numPoints(realCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
                avg += avgDistPointCenter(M.plane, numPoints(realCenters), metric);
            } else {
                vector<Point> intCenters = intPoints(randomCenters(n, k, false));
                int PIndex = 0;
                Matching M(n, k);
                CG.solveInt(intCenters, M, cutoff, PIndex);
                PHLL.solveInt(intCenters, M);
                for (int iter = 0; iter < KMeansIter; iter++) {
                    intCenters = intPoints(weightedCentroids(M.plane, numPoints(intCenters), metric, p, false));
                    PIndex = 0;
                    M = Matching(n, k);
                    CG.solveInt(intCenters, M, cutoff, PIndex);
                    PHLL.solveInt(intCenters, M);
                }
                if (checkStable) {
                    bool stable = M.isStable(numPoints(intCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
                avg += avgDistPointCenter(M.plane, numPoints(intCenters), metric);
            }
        }
        avg = avg/numRuns;
        cout << p << ",\t" << avg << endl;
        p += 0.1;
    }
}

void Benchmarker::runtimeExperiment(bool real, Num metr, bool checkStable) {

    vector<int> ns = getInts("100 200 300 400 500 600 700 800 900 1000");
//    ns = getInts("800 900 1000");
    Metric metric(metr);
    double cutoff = 0.15;
    int numRuns = 10;
    vector<BiGreedyMatcher*> bimatchers = getMatchers("PS PHEP PHLP PHLL PHEL");
//    bimatchers = getMatchers("PS");

    if (real) cout<<"real centers, metric "<<metr<<endl;
    else cout<<"int centers, metric "<<metr<<endl;

    cout<<"n,\tCG";
    for (BiGreedyMatcher* bimatcher : bimatchers) {
        bimatcher->setMetric(metric);
        cout<<",\t"<<bimatcher->acronym();
    }
    cout<<endl;
    for (int n : ns) {
        cout<<n;
        int k = 10*n;
        { //circle growing
            double avg = 0;
            for (int numRun = 0; numRun < numRuns; numRun++) {
                vector<Point> intCenters;
                vector<DPoint> realCenters;
                if (real) realCenters = doublePoints(randomCenters(n, k, true));
                else intCenters = intPoints(randomCenters(n, k, false));
                int PIndex = 0;
                Matching M(n, k);
                clock_t clock1 = clock();
                CircleGrower circleGrower(n, metric);
                if (real) circleGrower.solveReal(realCenters, M, 0, PIndex);
                else circleGrower.solveInt(intCenters, M, 0, PIndex);
                clock_t clock2 = clock();
                double time = elapsed(clock1, clock2);
                avg += time;
                if (checkStable) {
                    bool stable;
                    if (real) stable = M.isStable(numPoints(realCenters), metric, 0.001, true);
                    else stable = M.isStable(numPoints(intCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
            }
            avg = avg/numRuns;
            cout<<",\t"<<avg;
        }
        for (BiGreedyMatcher* bimatcher: bimatchers) {
            double avg = 0;
            for (int numRun = 0; numRun < numRuns; numRun++) {
                vector<Point> intCenters;
                vector<DPoint> realCenters;
                if (real) realCenters = doublePoints(randomCenters(n, k, true));
                else intCenters = intPoints(randomCenters(n, k, false));
                int PIndex = 0;
                Matching M(n, k);
                clock_t clock1 = clock();
                CircleGrower circleGrower(n, metric);
                if (real) circleGrower.solveReal(realCenters, M, cutoff, PIndex);
                else circleGrower.solveInt(intCenters, M, cutoff, PIndex);
                if (real) bimatcher->solveReal(realCenters, M);
                else bimatcher->solveInt(intCenters, M);
                clock_t clock2 = clock();
                double time = elapsed(clock1, clock2);
                avg += time;
                if (checkStable) {
                    bool stable;
                    if (real) stable = M.isStable(numPoints(realCenters), metric, 0.001, true);
                    else stable = M.isStable(numPoints(intCenters), metric, 0.001, true);
                    if (!stable) {
                        cout<<"not stable"<<endl;
                        exit(0);
                    }
                }
            }
            avg = avg/numRuns;
            cout<<",\t"<<avg;
        }
        cout<<endl;
    }
}



void Benchmarker::individualRunInt(int n, const CircleGrower& circleGrower, const vector<BiGreedyMatcher*>& bimatchers,
        const Metric& metric, const vector<Point>& centers, double cutoff) {


    Matching M(n, centers.size());
    int PIndex = 0;
    clock_t beginCG = clock();
    circleGrower.solveInt(centers, M, cutoff, PIndex);
    clock_t endCG = clock();
    double timeCG = elapsed(beginCG, endCG);

    if (cutoff == 0) {
        bool stable = M.isStable(numPoints(centers), metric, 0.01, true);
        if (!stable) cout<<"CG not stable"<<endl;

        cout<<"CG"<<" "<<0<<" ("<<timeCG<<")\n";
        for (BiGreedyMatcher* bimatcher : bimatchers) {
            cout<<bimatcher->acronym()<<" "<<0<<" ("<<timeCG<<")\n";
        }
    } else {
        {
            Matching MCopy(M);
            clock_t begin = clock();
            circleGrower.solveInt(centers, MCopy, 0, PIndex);
            clock_t end = clock();
            bool stable = MCopy.isStable(numPoints(centers), metric, 0.01, true);
            if (!stable) cout<<"CG2 not stable"<<endl;
            else {
                double time = elapsed(begin, end);
                cout<<"CG2"<<" "<<time<<" ("<<timeCG+time<<")\n";
            }
        }
        for (BiGreedyMatcher* bimatcher : bimatchers) {
            Matching MCopy(M);
            bimatcher->setMetric(metric);
            clock_t begin = clock();
            bimatcher->solveInt(centers, MCopy);
            clock_t end = clock();
            cout << bimatcher->acronym() << " ";
            cout.flush();
            bool stable = MCopy.isStable(numPoints(centers), metric, 0.01, true);
            if (not stable) {
                cout<<"not stable"<<endl;
            } else {
                double time = elapsed(begin, end);
                cout<<time<<" ("<<timeCG+time<<")\n";
            }
        }
    }
    cout<<endl;
}

void Benchmarker::individualRunReal(int n, const CircleGrower& circleGrower, const vector<BiGreedyMatcher*>& bimatchers,
        const Metric& metric, const vector<DPoint>& centers, double cutoff) {

    Matching M(n, centers.size());
    int PIndex = 0;
    clock_t beginCG = clock();
    circleGrower.solveReal(centers, M, cutoff, PIndex);
    clock_t endCG = clock();
    double timeCG = elapsed(beginCG, endCG);

    if (cutoff == 0) {
        bool stable = M.isStable(numPoints(centers), metric, 0.01, true);
        if (!stable) cout<<"CG not stable"<<endl;

        cout<<"CG"<<" "<<0<<" ("<<timeCG<<")\n";
        for (BiGreedyMatcher* bimatcher : bimatchers) {
            cout<<bimatcher->acronym()<<" "<<0<<" ("<<timeCG<<")\n";
        }
    } else {
        {
            Matching MCopy(M);
            clock_t begin = clock();
            circleGrower.solveReal(centers, MCopy, 0, PIndex);
            clock_t end = clock();
            bool stable = MCopy.isStable(numPoints(centers), metric, 0.01, true);
            if (!stable) cout<<"CG2 not stable"<<endl;
            else {
                double time = elapsed(begin, end);
                cout<<"CG2"<<" "<<time<<" ("<<timeCG+time<<")\n";
            }
        }
        for (BiGreedyMatcher* bimatcher : bimatchers) {
            Matching MCopy(M);
            bimatcher->setMetric(metric);
            clock_t begin = clock();
            bimatcher->solveReal(centers, MCopy);
            clock_t end = clock();
            bool stable = MCopy.isStable(numPoints(centers), metric, 0.01, true);
            cout<<bimatcher->acronym()<<" ";
            if (!stable) {
                cout<<"not stable"<<endl;
            } else {
                double time = elapsed(begin, end);
                cout<<time<<" ("<<timeCG+time<<")\n";
            }
        }
    }
    cout<<endl;
}



double Benchmarker::elapsed(clock_t begin, clock_t end) {
    return double(end - begin)/CLOCKS_PER_SEC;
}

vector<string> Benchmarker::split(string s) {
    vector<string> res;
    stringstream ss(s);
    string next;
    while (ss>>next) res.push_back(next);
    return res;
}

vector<Metric> Benchmarker::getMetrics(string s) {
    vector<Metric> res;
    for (string item : split(s)) {
        res.push_back(Metric(Num(item)));
    }
    return res;
}

vector<BiGreedyMatcher*> Benchmarker::getMatchers(string s) {
    vector<BiGreedyMatcher*> res;
    for (string item : split(s)) {
        if (item  == "PS") res.push_back(new PairSort());
        else if (item == "PHEP") res.push_back(new PairHeap(false, true));
        else if (item == "PHEL") res.push_back(new PairHeap(false, false));
        else if (item == "PHLP") res.push_back(new PairHeap(true, true));
        else if (item == "PHLL") res.push_back(new PairHeap(true, false));
        else cout<<"unknown matcher"<<endl;
    }
    return res;
}

vector<int> Benchmarker::getInts(string s) {
    vector<int> res;
    for (string item : split(s)) {
        res.push_back(Num(item).asInt());
    }
    return res;
}

vector<double>  Benchmarker::getDoubles(string s) {
    vector<double> res;
    for (string item : split(s)) {
        res.push_back(Num(item).asDouble());
    }
    return res;
}










//void Benchmarker::neighborHeuristicExperiment() {
//    qsrand(QTime::currentTime().msec());
//    int n = 1000;
//    double cutoff = 10000;
//    int numRuns = 10;
//    vector<Metric> metrics = getMetrics("2 1 inf");
//    vector<int> ks = getInts("1000 10000");
//    vector<vector<int>> latexRows(1);
//    for (int k : ks) {
//        for (Metric metric : metrics) {
//            DiskGrower matcher(n, metric);
//            int maxQueries = 0;
//            for (int i = 0; i < numRuns; i++) {
//                vector<Point> centers = intPoints(randomCenters(n, k, false));
//                vector<vector<int>> plane;
//                vector<int> quotas;
//                matcher.initEmpty(plane, quotas, k);
//                matcher.incrementalMatchingSiteCutoff(centers, plane, quotas, cutoff);

//                int count = 0;
//                for (int q: quotas) count += q>0;
//                cout<<count<<" ";

//                NeighborHeuristic NH(metric);
//                NH.solve(centers, plane, quotas);
//                maxQueries = max(NH.getNumNNQueries(), maxQueries);
//            }
//            cout<<endl;
//            latexRows[0].push_back(maxQueries);
//        }
//    }
//    printAsLatexTableRows(latexRows);
//    exit(0);
//}

//void Benchmarker::pairHeapNearestNeighborExperiment2() {
//    qsrand(QTime::currentTime().msec());
//    int n = 1000;
//    double cutoff = 10000;
//    int numRuns = 10;
//    vector<Metric> metrics = getMetrics("2 1 inf");
//    vector<int> ks = getInts("100 1000 10000");
//    for (int k : ks) {
//        cout << "k: " << k << endl;
//        for (Metric metric : metrics) {
//            cout << "metric: " << metric << endl;
//            DiskGrower matcher(n, metric);
//            for (int i = 0; i < numRuns; i++) {
//                vector<Point> centers = intPoints(randomCenters(n, k, false));
//                vector<vector<int>> plane;
//                vector<int> quotas;
//                matcher.initEmpty(plane, quotas, k);
//                matcher.incrementalMatchingSiteCutoff(centers, plane, quotas, cutoff);

//                int remCenters = 0;
//                for (int q: quotas) remCenters += q>0;
//                SiteHeapNN SHNN(metric);
//                SHNN.solve(centers, plane, quotas);
//                cout<<remCenters<<" "<<SHNN.getAlpha() << "   ";

//            }
//            cout<<endl;
//        }
//    }
//    exit(0);
//}

//void Benchmarker::pairHeapNearestNeighborExperiment() {
//    qsrand(QTime::currentTime().msec());
//    int n = 100;
//    int numRuns = 10;
//    vector<Metric> metrics = getMetrics("2 1 inf");
//    vector<int> ks = getInts("10 100 1000");
//    DiskGrower matcher(n, metrics[0]);
//    for (int k : ks) {
//        cout << "k: " << k << endl;
//        for (Metric metric : metrics) {
//            cout << "metric: " << metric << endl;
//            for (int i = 0; i < numRuns; i++) {
//                vector<Point> centers = intPoints(randomCenters(n, k, false));
//                vector<vector<int>> plane;
//                vector<int> quotas;
//                matcher.initEmpty(plane, quotas, k);
//                SiteHeapNN SHNN(metric);
//                SHNN.solve(centers, plane, quotas);
//                cout << SHNN.getAlpha() << " ";
//            }
//            cout<<endl;
//        }
//    }
//    exit(0);
//}

//void Benchmarker::cutoffExperimentReal() {
//    int n = 1000;
//    vector<int> ks = getInts("100 1000 10000");
//    vector<double> cutoffs = getDoubles("0 0.025 0.05 0.075 0.1 0.125 0.15 0.175 0.2 0.225 0.25");
//    Metric metric(2);
//    int numRuns = 10;

//    vector<vector<int>> plane;
//    vector<int> quotas;

//    clock_t initPlane1 = clock();
//    DiskGrower circleGrower(n, metric);
//    clock_t initPlane2 = clock();
//    SiteHeap pairHeap(metric);
//    SiteHeapNN pairHeapNN(metric);

//    cout<<"cutoff, PH100, PHNN100, PH1k, PHNN1k, PH10k, PHNN10k"<<endl;
//    for (double cutoff : cutoffs) {
//        cout<<cutoff;
//        for (int k : ks) {
//            double avg = 0, avgNN = 0;
//            for (int numRun = 0; numRun < numRuns; numRun++) {
//                {
//                    vector<DPoint> centers = doublePoints(randomCenters(n, k, true));
//                    circleGrower.initEmpty(plane, quotas, k);
//                    int PIndex = 0;
//                    clock_t circleGrower1 = clock();
//                    circleGrower.fuzzyMatching(centers, plane, quotas, cutoff, PIndex);
//                    clock_t circleGrower2 = clock();
//                    clock_t pairHeap1 = clock();
//                    pairHeap.solve(centers, plane, quotas);
//                    clock_t pairHeap2 = clock();
//                    double totalTime = elapsed(initPlane1, initPlane2) +
//                                       elapsed(circleGrower1, circleGrower2) +
//                                       elapsed(pairHeap1, pairHeap2);
//                    avg += totalTime;
//                    bool stable = isStable(plane, numPoints(centers), metric, 0.001, true);
//                    if (!stable) {
//                        cout<<"pair heap not stable"<<endl;
//                        exit(0);
//                    }
//                }
//                {
//                    vector<DPoint> centers = doublePoints(randomCenters(n, k, true));
//                    circleGrower.initEmpty(plane, quotas, k);
//                    int PIndex = 0;
//                    clock_t circleGrower1 = clock();
//                    circleGrower.fuzzyMatching(centers, plane, quotas, cutoff, PIndex);
//                    clock_t circleGrower2 = clock();
//                    clock_t pairHeapNN1 = clock();
//                    pairHeapNN.solve(centers, plane, quotas);
//                    clock_t pairHeapNN2 = clock();
//                    double totalTime = elapsed(initPlane1, initPlane2) +
//                                       elapsed(circleGrower1, circleGrower2) +
//                                       elapsed(pairHeapNN1, pairHeapNN2);
//                    avgNN += totalTime;
//                    bool stable = isStable(plane, numPoints(centers), metric, 0.001, true);
//                    if (!stable) {
//                        cout<<"pair heap NN not stable"<<endl;
//                        exit(0);
//                    }
//                }
//            }
//            avg /= numRuns;
//            avgNN /= numRuns;
//            cout<<", "<<avg<<", "<<avgNN;
//        }
//        cout<<endl;
//    }
//    exit(0);
//}
