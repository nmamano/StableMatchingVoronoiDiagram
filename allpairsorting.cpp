#include "allpairsorting.h"
#include "radixsort.h"
#include <unordered_map>

void AllPairSorting::solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const
{
    int n = plane.size();
    int numRemPoints = 0;
    for (int quota : quotas) numRemPoints += quota;
    if (numRemPoints == 0) return;

    vector<int> remCIds = remainingCenterIds(quotas);
    vector<Point> remPoints = remainingPoints(plane, numRemPoints);

    vector<Link> L = linksSortedByLength(n, centers, remPoints, remCIds, metric);

    for (const Link& link : L) {
        int i = link.i, j = link.j, c_id = link.cId;
        if (quotas[c_id] > 0 and plane[i][j] == -1) {
            plane[i][j] = c_id;
            quotas[c_id]--;
            numRemPoints--;
            if (numRemPoints == 0) break;
        }
    }
    if (numRemPoints != 0) {
        cout << "APS Error: unasigned points" << endl;
    }
}

vector<Link> AllPairSorting::linksSortedByLength(int n,
        const vector<Point>& centers, const vector<Point>& remPoints,
        const vector<int>& remCIds, const Metric& metric) {

    if (metric.val == 2) { //optimized case
        return linksSortedByLengthL2(n, centers, remPoints, remCIds);
    }
    return linksSortedByLengthGenCase(centers, remPoints, remCIds, metric);

}

vector<Link> AllPairSorting::linksSortedByLengthL2(int n, const vector<Point>& centers,
        const vector<Point>& remPoints, const vector<int>& remCIds) {
    //counting sort
    vector<int> counts(n*n*2, 0);
    for (const Point& p : remPoints) {
        for (int c_id : remCIds) {
            Point c = centers[c_id];
            int disSq = (p.i-c.i)*(p.i-c.i)+(p.j-c.j)*(p.j-c.j);
            counts[disSq]++;
        }
    }

    int total = 0;
    for (int i = 0; i < n*n*2; i++) {
        int oldCount = counts[i];
        counts[i] = total;
        total += oldCount;
    }

    vector<Link> res(remCIds.size()*remPoints.size());
    for (const Point& p : remPoints) {
        for (int c_id : remCIds) {
            Point c = centers[c_id];
            int disSq = (p.i-c.i)*(p.i-c.i)+(p.j-c.j)*(p.j-c.j);
            res[counts[disSq]] = Link(p.i, p.j, c_id);
            counts[disSq]++;
        }
    }
    return res;
}

vector<Link> AllPairSorting::linksSortedByLengthGenCase(
        const vector<Point>& centers, const vector<Point>& remPoints,
        const vector<int>& remCIds, const Metric& metric) {

    if (metric.hasIntDists()) {
        unordered_map<int, int> dist2Count;
        for (const Point& p : remPoints) {
            for (int c_id : remCIds) {
                Point c = centers[c_id];
                int dis = metric.iSortValue(p, c);
                dist2Count[dis]++;
            }
        }
        vector<int> dists;
        dists.reserve(dist2Count.size());
        for (auto& distCount : dist2Count) {
            dists.push_back(distCount.first);
        }
        radix_sort(dists);

        int total = 0;
        for (int dis : dists) {
            int oldCount = dist2Count[dis];
            dist2Count[dis] = total;
            total += oldCount;
        }
        vector<Link> res(remCIds.size()*remPoints.size());
        for (const Point& p : remPoints) {
            for (int c_id : remCIds) {
                Point c = centers[c_id];
                int dis = metric.iSortValue(p, c);
                int index = dist2Count[dis];
                res[index] = Link(p.i, p.j, c_id);
                dist2Count[dis]++;
            }
        }
        return res;
    } else {
        unordered_map<double, int> dist2Count;
        for (const Point& p : remPoints) {
            for (int c_id : remCIds) {
                Point c = centers[c_id];
                double dis = metric.dSortValue(p, c);
                dist2Count[dis]++;
            }
        }
        vector<double> dists;
        dists.reserve(dist2Count.size());
        for (auto& distCount : dist2Count) {
            dists.push_back(distCount.first);
        }
        sort(dists.begin(), dists.end());

        int total = 0;
        for (double dis : dists) {
            int oldCount = dist2Count[dis];
            dist2Count[dis] = total;
            total += oldCount;
        }

        vector<Link> res(remCIds.size()*remPoints.size());
        for (const Point& p : remPoints) {
            for (int c_id : remCIds) {
                Point c = centers[c_id];
                double dis = metric.dSortValue(p, c);
                int index = dist2Count[dis];
                res[index] = Link(p.i, p.j, c_id);
                dist2Count[dis]++;
            }
        }
        //check correct
//        if (res[0].c_id == -1) cout << "uninit link " << 0 << " ";
//        for (int i = 1; i < (int)res.size(); i++) {
//            Link prev = res[i-1];
//            Link curr = res[i];
//            if (curr.c_id == -1) cout << "uninit link " << i << " ";
//            else {
//                double disPrev = metric.dSortValue(Point(prev.i, prev.j), centers[prev.c_id]);
//                double disCurr = metric.dSortValue(Point(curr.i, curr.j), centers[curr.c_id]);
//                if (disPrev > disCurr) {
//                    cout<<"APS Error: links not sorted "<<i<<" "<<prev<<" "<<curr<<" "<<disPrev<<" "<<disCurr<<endl;
//                }
//            }
//        }
//        cout<<endl;
        return res;
    }
}
