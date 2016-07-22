#ifndef BENCHMARKER_H
#define BENCHMARKER_H
#include <ctime>
#include <vector>
#include "matchingutils.h"
#include "matcher.h"
#include "metric.h"

class Benchmarker
{
public:
    Benchmarker();
    void run();
private:
    double elapsed(clock_t begin, clock_t end);

    void individualRun(const Matcher &matcher, const Metric &metric, const vector<Point> &centers, double cutoff);
};

#endif // BENCHMARKER_H
