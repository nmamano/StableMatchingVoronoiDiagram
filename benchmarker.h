#ifndef BENCHMARKER_H
#define BENCHMARKER_H
#include <ctime>
#include <vector>
#include "matchingutils.h"
#include "diskgrower.h"
#include "metric.h"

class Benchmarker
{
public:
    Benchmarker();
    void run();
private:
    double elapsed(clock_t begin, clock_t end);

    void individualRun(const DiskGrower &matcher, const vector<BiGreedyMatcher *> &bimatchers, const Metric &metric, const vector<Point> &centers, double cutoff);
};

#endif // BENCHMARKER_H
