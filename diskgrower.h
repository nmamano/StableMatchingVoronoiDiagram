#ifndef DISKGROWER_H
#define DISKGROWER_H

#include <vector>
#include "matchingutils.h"
#include "bigreedymatcher.h"
#include "dpoint.h"

using namespace std;

class DiskGrower
{
public:
    DiskGrower(int n, Metric metric = Num(2));
    vector<vector<int>> query(const vector<Point>& centers, double cutoff = 0.005) const;
    void incrementalMatching(const vector<Point> &centers, vector<vector<int> > &plane,
                             vector<int> &quotas, double cutoff, int &iter) const;

    int tailoredCutoff(int k) const;

    void setN(int newN);
    void setMetric(Metric newMetric);
    Metric getMetric() {return metric;}
    void step(vector<vector<int>> &plane, const vector<Point> &centers, int iter);
    void initEmpty(vector<vector<int> > &plane, vector<int> &quotas, int k) const;

    vector<vector<int> > query(const vector<DPoint> &centers) const;
private:
    int n;
    Metric metric;
    BiGreedyMatcher* bimatcher;
    vector<Point> P;

    void buildP();


    void innerLoop(int pi, int pj,
            const vector<int>& remCIds, const vector<Point>& centers,
            vector<vector<int>>& plane, vector<int>& quotas) const;

    void middleLoop(int i, int j,
            const vector<int> &remCIds, const vector<Point> &centers,
            vector<vector<int> > &plane, vector<int> &quotas) const;

    static vector<Point> pointsSortedByDistOrigin(int n, const Metric &metric);
    static vector<Point> pointsSortedByDistOriginL2(int n);
    static vector<Point> pointsSortedByDistOriginGenCase(int n, const Metric &metric);


    struct LengthIndex {
        double length;
        int index;
        bool operator<(const LengthIndex& rhs) const {
            return length < rhs.length;
        }
    };


    void sortLinks(vector<Link> &links, const vector<DPoint> &centers) const;
    void addLinks(int pi, int pj, const vector<int> &remCIds, const vector<vector<int> > &plane, const vector<DPoint> &centers, const vector<Point> &cps, vector<Link> &chunkLinks, double lengthThreshold) const;
};

#endif // DISKGROWER_H