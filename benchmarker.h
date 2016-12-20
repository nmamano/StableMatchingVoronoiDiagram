#ifndef BENCHMARKER_H
#define BENCHMARKER_H
#include <ctime>
#include <vector>
#include "matchingutils.h"
#include "circlegrower.h"
#include "metric.h"

class Benchmarker
{
public:
    static void runExperiments();


private:
    static void centroidExperiment(bool real, Num metr, bool checkStable);
    static void cutoffExperiment(bool real, Num metr, bool checkStable);
    static void runtimeExperiment(bool real, Num metr, bool checkStable);
    static void profileExperiment(bool real, Num metr, bool checkStable);
    static void compAlgorithms(bool real);

    static double elapsed(clock_t begin, clock_t end);

    static void individualRunInt(int n, const CircleGrower &circleGrower, const vector<BiGreedyMatcher *> &bimatchers, const Metric &metric, const vector<Point> &centers, double cutoff);
    static void individualRunReal(int n, const CircleGrower &circleGrower, const vector<BiGreedyMatcher *> &bimatchers, const Metric &metric, const vector<DPoint> &centers, double cutoff);

    static vector<double> getDoubles(string s);
    static vector<int> getInts(string s);
    static vector<BiGreedyMatcher *> getMatchers(string s);
    static vector<Metric> getMetrics(string s);
    static vector<string> split(string s);

};

#endif // BENCHMARKER_H
