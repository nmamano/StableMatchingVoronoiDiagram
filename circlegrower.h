#ifndef CIRCLEGROWER_H
#define CIRCLEGROWER_H

#include <vector>
#include "matchingutils.h"
#include "bigreedymatcher.h"
#include "dpoint.h"
#include "matching.h"

using namespace std;

class CircleGrower
{
public:
    CircleGrower(int n, Metric metric = Num(2));

    void setN(int newN);
    void setMetric(Metric newMetric);
    Metric getMetric() {return metric;}

    vector<vector<int>> combinedApproachReal(const vector<DPoint> &centers, int appetite) const;
    vector<vector<int>> combinedApproachInt(const vector<Point>  &centers, int appetite) const;

    void solveInt(const vector<Point> &centers, Matching &M, double cutoff, int &PIndex) const;

    void solveReal(const vector<DPoint> &centers, Matching &M, double cutoff, int &PIndex) const;

    //to show matching constructiong step by step
    void step(vector<vector<int>> &plane, const vector<Point> &centers, int iter);

    //for experiments requiring a site cutoff
    void solveIntSiteCutoff(const vector<Point> &centers, Matching& M, int cutoff) const;

    double idealAvgDistPointCenter(int regionSize);

private:
    int n;
    Metric metric;
    vector<Point> P;

    void buildP();
    static vector<Point> pointsSortedByNorm(int n, const Metric &metric);
    static vector<Point> pointsSortedByL2Norm(int n);
    static vector<Point> pointsSortedByRealNorm(int n, const Metric &metric);
    static vector<Point> pointsSortedByIntNorm(int n, const Metric &metric);


    void sortLinks(vector<Link> &links, const vector<DPoint> &centers) const;
    void addLinksPPoint(int i, int j, const vector<int> &cIds, const vector<vector<int> > &plane, const vector<Point> &cps, vector<Link> &links, const vector<DPoint> &centers, double lengthThreshold) const;
    void addLinksPPoint(int i, int j, const vector<int> &cIds, const vector<vector<int> > &plane, const vector<Point> &cps, vector<Link> &links) const;
};

#endif // CIRCLEGROWER_H
