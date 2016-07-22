#ifndef MATCHER_H
#define MATCHER_H

#include <vector>
#include <queue>
#include "matchingutils.h"

using namespace std;

class Matcher
{
public:
    Matcher(int n, Metric metric = Num(2));
    vector<vector<int>> query(const vector<Point>& centers, double cutoff = 0.005) const;
    void incrementalMatching(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas, double cutoff) const;
    void allPairSortingMatching(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const;
    void nearestNeighborMatching(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const;
    void siteHeapMatching(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const;

    int tailoredCutoff(int k) const;

    void setN(int newN);
    void setMetric(Metric newMetric);
    Metric getMetric() {return metric;}
    void step(vector<vector<int>> &plane, const vector<Point> &centers, int iter);
private:
    int n;
    Metric metric;
    vector<Point> P;

    void buildP();


    void innerLoop(int pi, int pj,
            const vector<int>& c_ids, const vector<Point>& centers,
            vector<vector<int>>& plane, vector<int>& quotas) const;

    void middleLoop(int i, int j,
            const vector<int> &c_ids, const vector<Point> &centers,
            vector<vector<int> > &plane, vector<int> &quotas) const;

    static vector<Point> remainingPoints(const vector<vector<int> > &plane, int num_rem_points);
    static vector<int> remainingCenterIds(const vector<int> &quotas);

    static vector<Point> pointsSortedByDistOrigin(int n, const Metric &metric);
    static vector<Point> pointsSortedByDistOriginL2(int n);
    static vector<Point> pointsSortedByDistOriginGenCase(int n, const Metric &metric);

    struct Link {
        int i,j;
        int c_id;
        Link(int i, int j, int c_id):
            i(i), j(j), c_id(c_id) { }
        Link() { }
    };

    static vector<Link> linksSortedByLength(int n, const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds, const Metric &metric);
    static vector<Link> linksSortedByLengthL2(int n, const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds);
    static vector<Link> linksSortedByLengthGenCase(const vector<Point> &centers, const vector<Point> &remPoints, const vector<int> &remCIds, const Metric &metric);

    static void radix_sort(vector<int> &array);
    static void radix_sort(vector<int> &array, int offset, int end, int shift);
    static void insertion_sort(vector<int> &array, int offset, int end);

    int findIndexClosest(const Point &q, const vector<Point> &ps) const;
    int cIdIndexWithClosestNeighbor(const vector<Point> &centers, const vector<int> &remCIds, const vector<Point> &remPoints, const vector<int> &neighbors) const;
    int cIdIndexWithClosestNeighbor(const vector<int>& remCIds, const vector<double> &dists) const;


    template<class T> using min_heap = priority_queue<T, vector<T>, greater<T>>;

    struct SiteCenter
    {
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

#endif // MATCHER_H
