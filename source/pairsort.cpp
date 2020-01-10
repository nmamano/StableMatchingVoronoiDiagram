#include "pairsort.h"
#include "radixsort.h"

void PairSort::solveInt(const vector<Point> &centers, Matching& M) {
    if (metric.hasIntDists()) {
        intCenters = centers;
        real = false;
    } else {
        realCenters = vector<DPoint>(centers.begin(), centers.end());
        real = true;
    }
    solve(M);
}

void PairSort::solveReal(const vector<DPoint> &centers, Matching &M) {
    realCenters = centers;
    real = true;
    solve(M);
}

void PairSort::solve(Matching& M) {
    vector<int> remCIds = M.remainingCIds();
    vector<Point> remSites = M.remainingSites();
    if (remSites.empty()) return;

    vector<Link> L = linksSortedByLength(remSites, remCIds, M.n());

    processLinks(L, M, remSites.size());
}

void PairSort::processLinks(vector<Link> L, Matching& M, int numRemSites) {
    for (const Link& link : L) {
        int i = link.i, j = link.j, cId = link.cId;
        if (M.quotas[cId] > 0 and M.plane[i][j] == -1) {
            M.plane[i][j] = cId;
            M.quotas[cId]--;
            numRemSites--;
            if (numRemSites == 0) break;
        }
    }
    if (numRemSites != 0) cout << "PS Error: unasigned points" << endl;
}

vector<Link> PairSort::linksSortedByLength(const vector<Point>& remPoints, const vector<int>& remCIds, int n) {

    if (!real && metric.val == 2) {
        return linksSortedByLengthL2(n, intCenters, remPoints, remCIds);
    } else if (!real) {
        return linksSortedByLengthIntDists(intCenters, remPoints, remCIds, metric);
    } else {
        return linksSortedByLengthRealDists(realCenters, remPoints, remCIds, metric);
    }
}

vector<Link> PairSort::linksSortedByLengthL2(int n, const vector<Point>& centers,
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

vector<Link> PairSort::linksSortedByLengthIntDists(
        const vector<Point>& centers, const vector<Point>& remPoints,
        const vector<int>& remCIds, const Metric& metric) {

    int numRemPoints = remPoints.size();
    int numRemCenters = remCIds.size();
    int numCenters = centers.size();
    int numLinks = numRemPoints*numRemCenters;
    vector<IdIntDist> dists(numLinks);
    int index = 0;
    for (int pInd = 0; pInd < numRemPoints; pInd++) {
        Point p = remPoints[pInd];
        for (int cId : remCIds) {
            Point c = centers[cId];
            dists[index].id = pInd*numCenters+cId;
            dists[index].dist = metric.iSortValue(p, c);
            index++;
        }
    }
    radix_sort(dists);
    vector<Link> res(numLinks);
    for (int i = 0; i < numLinks; i++) {
        int pInd = dists[i].id/numCenters;
        int cId = dists[i].id%numCenters;
        Point p = remPoints[pInd];
        res[i] = Link(p.i, p.j, cId);
    }
    return res;
}

vector<Link> PairSort::linksSortedByLengthRealDists(
        const vector<DPoint>& centers, const vector<Point>& remPoints,
        const vector<int>& remCIds, const Metric& metric) {

    int numRemPoints = remPoints.size();
    int numRemCenters = remCIds.size();
    int numCenters = centers.size();
    int numLinks = numRemPoints*numRemCenters;
    vector<IdRealDist> dists(numLinks);
    int index = 0;
    for (int pInd = 0; pInd < numRemPoints; pInd++) {
        DPoint p = remPoints[pInd];
        for (int cId : remCIds) {
            DPoint c = centers[cId];
            dists[index].id = pInd*numCenters+cId;
            dists[index].dist = metric.dSortValue(p, c);
            index++;
        }
    }
    sort(dists.begin(), dists.end());
    vector<Link> res(numLinks);
    for (int i = 0; i < numLinks; i++) {
        int pInd = dists[i].id/numCenters;
        int cId = dists[i].id%numCenters;
        Point p = remPoints[pInd];
        res[i] = Link(p.i, p.j, cId);
    }
    return res;
}
