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

    vector<vector<int>> realCentersQuery(const vector<DPoint> &centers) const;
    vector<vector<int>> intCentersQuery( const vector<Point>  &centers, double cutoff = 0.005) const;
    void incrementalMatching(const vector<Point> &centers, vector<vector<int> > &plane,
                             vector<int> &quotas, double cutoff, int &iter) const;

    int tailoredCutoff(int k) const;

    void setN(int newN);
    void setMetric(Metric newMetric);
    Metric getMetric() {return metric;}
    void step(vector<vector<int>> &plane, const vector<Point> &centers, int iter);
    void initEmpty(vector<vector<int> > &plane, vector<int> &quotas, int k) const;

private:
    int n;
    Metric metric;
    BiGreedyMatcher* bimatcher;
    vector<Point> P;

    void buildP();


    void processGridPoint(int pi, int pj,
            const vector<int>& remCIds, const vector<Point>& centers,
            vector<vector<int>>& plane, vector<int>& quotas) const;

    void processPPoint(int i, int j,
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
    void addLinksGridPoint(int pi, int pj, const vector<int> &cIds, const vector<vector<int> > &plane, const vector<DPoint> &centers, const vector<Point> &cps, vector<Link> &links, double lengthThreshold) const;
    void addLinksPPoint(int i, int j, const vector<int> &cIds, const vector<vector<int> > &plane, const vector<DPoint> &centers, const vector<Point> &cps, vector<Link> &links, double lengthThreshold) const;
};

#endif // DISKGROWER_H
