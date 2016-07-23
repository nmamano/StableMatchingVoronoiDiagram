#ifndef SITEHEAP_H
#define SITEHEAP_H

#include "bigreedymatcher.h"
#include <queue>
using namespace std;

class SiteHeap : public BiGreedyMatcher
{
public:
    SiteHeap(): BiGreedyMatcher() {}
    SiteHeap(const Metric& metric): BiGreedyMatcher(metric) {}
    string acronym() const { return "SH"; }

    void solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const;
private:
    template<class T> using min_heap = priority_queue<T, vector<T>, greater<T>>;

    struct SiteCenter {
        int siteInd;
        int cId;
        double dist;
        SiteCenter(int siteInd, int cId, double dist): siteInd(siteInd), cId(cId), dist(dist) {}
        bool operator<(const SiteCenter& rhs) const {
            return dist < rhs.dist;
        }
        bool operator>(const SiteCenter& rhs) const {
            return dist > rhs.dist;
        }
        SiteCenter(): siteInd(-1), cId(-1), dist(0) {}
        friend ostream& operator<<(ostream& out, const SiteCenter& sc) {
            out << "(" << sc.siteInd << "," << sc.cId << "," << sc.dist << ")";
            return out;
        }
    };

    vector<SiteCenter> centersSortedByDist(const vector<Point> &centers, const Point &q, int qInd, const vector<int> &cIds) const;
    static vector<int> removeMatchedPoints(const vector<int> &remPointInds, const vector<bool> &matched);
    static void advanceInds(const vector<int> &quotas, const vector<int> &remPointInds, const vector<vector<SiteCenter> > &vps, vector<int> &vpInds);
    static min_heap<SiteCenter> buildHeap(const vector<int> &remPointInds, const vector<vector<SiteCenter> > &vps, const vector<int> &vpInds);

};

#endif // SITEHEAP_H
