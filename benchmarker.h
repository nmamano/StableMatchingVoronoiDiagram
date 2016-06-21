#ifndef BENCHMARKER_H
#define BENCHMARKER_H
#include <ctime>
#include <vector>
#include "matchingutils.h"

class Benchmarker
{
public:
    Benchmarker();
    void run();
private:
    double elapsed(clock_t begin, clock_t end);

};

#endif // BENCHMARKER_H
