#include "circlegrower.h"
#include "radixsort.h"
#include "pairsort.h"
#include "matchingutils.h"
#include "pairheap.h"
#include <vector>
#include <math.h>
#include <algorithm>

using namespace std;

CircleGrower::CircleGrower(int n, Metric metric):
    n(n), metric(metric)
{
    buildP();
}

void CircleGrower::setN(int newN) {
    if (newN == n) return;
    n = newN;
    if ((int)P.size() < n*(n+1)/2) buildP();
}

void CircleGrower::setMetric(Metric newMetric) {
    if (newMetric == metric) return;
    metric = newMetric;
    buildP();
}

void CircleGrower::buildP() {
    P = pointsSortedByNorm(n, metric);
}

vector<Point> CircleGrower::pointsSortedByNorm(int n, const Metric& metric) {
    if (metric.val==2) {
        return pointsSortedByL2Norm(n);
    } else if (metric.hasIntDists()) {
        return pointsSortedByIntNorm(n, metric);
    } else {
        return pointsSortedByRealNorm(n, metric);
    }
}

vector<Point> CircleGrower::pointsSortedByL2Norm(int n) {
    //counting sort
    int maxDist = (n-1)*(n-1)*2+1;
    vector<int> counts(maxDist);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            int disSq = i*i+j*j;
            counts[disSq]++;
        }
    }
    int total = 0;
    for (int i = 0; i < maxDist; i++) {
        int oldCount = counts[i];
        counts[i] = total;
        total += oldCount;
    }
    vector<Point> res(n*(n+1)/2);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            int disSq = i*i+j*j;
            res[counts[disSq]] = Point(i, j);
            counts[disSq]++;
        }
    }
    return res;
}

vector<Point> CircleGrower::pointsSortedByIntNorm(int n, const Metric& metric) {
    int numPoints = n*(n+1)/2;
    vector<IdIntDist> dists(numPoints);
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            dists[index].dist = metric.iSortValue(Point(i, j), Point(0, 0));
            dists[index].id = i*n+j;
            index++;
        }
    }
    radix_sort(dists);
    vector<Point> res(numPoints);
    for (int i = 0; i < numPoints; i++) {
        res[i] = Point(dists[i].id/n, dists[i].id%n);
    }
    return res;
}

vector<Point> CircleGrower::pointsSortedByRealNorm(int n, const Metric& metric) {
    int numPoints = n*(n+1)/2;
    vector<IdRealDist> dists(numPoints);
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            dists[index].dist = metric.dSortValue(Point(i, j), Point(0, 0));
            dists[index].id = i*n+j;
            index++;
        }
    }
    sort(dists.begin(), dists.end());
    vector<Point> res(numPoints);
    for (int i = 0; i < numPoints; i++) {
        res[i] = Point(dists[i].id/n, dists[i].id%n);
    }
    return res;
}

vector<vector<int>> CircleGrower::combinedApproachInt(
        const vector<Point> &centers, int appetite) const {
    int k = centers.size();
    Matching M(n, k, appetite);
    int PIndex = 0;
    double cutoff = 0.1;
    solveInt(centers, M, cutoff, PIndex);
    PairHeap pairHeap(metric, true, false);
    pairHeap.solveInt(centers, M);
    return M.plane;
}

vector<vector<int>> CircleGrower::combinedApproachReal(
        const vector<DPoint>& centers, int appetite) const {
    int k = centers.size();
    Matching M(n, k, appetite);
    int PIndex = 0;
    double cutoff = 0.05;
//    cerr<<"solve1"<<endl;
    solveReal(centers, M, cutoff, PIndex);
//    PairHeap pairHeap(metric, true, false);
    PairSort pairSort(metric);
//    cerr<<"solve2"<<endl;
//    pairHeap.solveReal(centers, M);
    pairSort.solveReal(centers, M);
//    cerr<<"solved"<<endl;
    return M.plane;
}

void CircleGrower::solveInt(const vector<Point> &centers,
        Matching& M, double cutoff, int& PIndex) const {

    vector<int> remCIds = M.remainingCIds();
    if (remCIds.empty()) return;

    int innerIter = 0;
    while (PIndex < (int)P.size()) {
        const Point& p = P[PIndex];
        int i = p.i, j = p.j;
        int iTrans[8] = {i, i, -i, -i, j, j, -j, -j};
        int jTrans[8] = {j, -j, j, -j, i, -i, i, -i};
        for (int t  = 0; t < 8; t++) {
            for (int cId : remCIds) {
                int si = iTrans[t] + centers[cId].i;
                int sj = jTrans[t] + centers[cId].j;
                if (si>=0 && sj>=0 && si<n && sj<n &&
                        M.plane[si][sj] == -1 && M.quotas[cId]>0) {
                    M.plane[si][sj] = cId;
                    M.quotas[cId]--;
                }
            }
        }
        PIndex++;
        innerIter += 8*remCIds.size();

        if (PIndex%64 == 0) { //update data structs from time to time
            vector<int> newCIds(0);
            newCIds.reserve(remCIds.size());
            int numRemPoints = 0;
            for (int cId : remCIds) {
                if (M.quotas[cId] > 0) {
                    newCIds.push_back(cId);
                    numRemPoints += M.quotas[cId];
                }
            }
            remCIds = move(newCIds);

            int remCenters = remCIds.size();
            int remLinks = remCenters * numRemPoints;
            if (numRemPoints == 0 || (float) remLinks/innerIter < cutoff) {
                return;
            }
        }
    }
    return;
}


void CircleGrower::solveReal(const vector<DPoint>& centers,
        Matching& M, double cutoff, int& PIndex) const {

    vector<int> remCIds = M.remainingCIds();
    if (remCIds.empty()) return;

    int k = M.k();

    vector<Point> cps(k);
    for (int i = 0; i < k; i++) {
        DPoint center = centers[i];
        cps[i] = Point(round(center.i), round(center.j));
    }

    double maxCpDist = 0;
    for (int cId = 0; cId < k; cId++) {
        if (M.quotas[cId] > 0) {
            double d = metric.ddist(centers[cId], cps[cId]);
            if (d > maxCpDist) maxCpDist = d;
        }
    }

    int numPoints = P.size();
    int innerIter = 0;
    while (remCIds.size() > 0) {
        int chunkEnd = min(PIndex+n/8, numPoints);
        double disLast = metric.ddist(P[chunkEnd-1], Point(0, 0));
        double disThreshold = disLast + 2*maxCpDist;

        int extrasEnd = chunkEnd;
        while (extrasEnd < numPoints &&
               metric.ddist(P[extrasEnd], Point(0, 0)) < disThreshold) {
            extrasEnd++;
        }
        //generate all the links in the chunk
        vector<Link> chunkLinks;
        int numPotentialLinks = 8*(extrasEnd-PIndex)*remCIds.size();
        chunkLinks.reserve(numPotentialLinks);

        innerIter += numPotentialLinks;

        double lengthThreshold = disLast + maxCpDist;
        for (int i = PIndex; i < chunkEnd; i++) {
            addLinksPPoint(P[i].i, P[i].j, remCIds, M.plane, cps, chunkLinks);
        }
        for (int i = chunkEnd; i < extrasEnd; i++) {
            addLinksPPoint(P[i].i, P[i].j, remCIds, M.plane, cps, chunkLinks, centers, lengthThreshold);
        }
        sortLinks(chunkLinks, centers);

        //process links in order
        for (const Link& link : chunkLinks) {
            int i = link.i, j = link.j, cId = link.cId;
            if (M.quotas[cId] > 0 and M.plane[i][j] == -1) {
                M.plane[i][j] = cId;
                M.quotas[cId]--;
            }
        }

        //update remaining centers and maxCpDist
        maxCpDist = 0;
        vector<int> newCIds(0);
        newCIds.reserve(remCIds.size());
        int numRemPoints = 0;
        for (int cId : remCIds) {
            if (M.quotas[cId] > 0) {
                numRemPoints += M.quotas[cId];
                newCIds.push_back(cId);
                double d = metric.ddist(centers[cId], cps[cId]);
                if (d > maxCpDist) maxCpDist = d;
            }
        }
        remCIds = move(newCIds);

        //move to next chunk
        PIndex = chunkEnd;

        int remCenters = remCIds.size();
        int remLinks = remCenters * numRemPoints;
        if (numRemPoints == 0 || (float) remLinks/innerIter < cutoff) {
            return;
        }
    }
}

void CircleGrower::addLinksPPoint(int i, int j, const vector<int>& cIds,
    const vector<vector<int>>& plane,
    const vector<Point>& cps, vector<Link>& links) const {

    int iTrans[8] = {i, i, -i, -i, j, j, -j, -j};
    int jTrans[8] = {j, -j, j, -j, i, -i, i, -i};
    for (int t  = 0; t < 8; t++) {
        for (int cId : cIds) {
            int si = iTrans[t] + cps[cId].i;
            int sj = jTrans[t] + cps[cId].j;
            if (si>=0 && sj>=0 && si<n && sj<n && plane[si][sj] == -1) {
                links.push_back(Link(si, sj, cId));
            }
        }
    }
}

void CircleGrower::addLinksPPoint(int i, int j, const vector<int>& cIds,
    const vector<vector<int>>& plane, const vector<Point>& cps, vector<Link>& links,
    const vector<DPoint>& centers, double lengthThreshold) const {

    int iTrans[8] = {i, i, -i, -i, j, j, -j, -j};
    int jTrans[8] = {j, -j, j, -j, i, -i, i, -i};
    for (int t  = 0; t < 8; t++) {
        for (int cId : cIds) {
            int si = iTrans[t] + cps[cId].i;
            int sj = jTrans[t] + cps[cId].j;
            if (si>=0 && sj>=0 && si<n && sj<n && plane[si][sj] == -1) {
                double length = metric.ddist(Point(si, sj), centers[cId]);
                if (length < lengthThreshold) {
                    links.push_back(Link(si, sj, cId));
                }
            }
        }
    }
}

void CircleGrower::sortLinks(vector<Link>& links, const vector<DPoint>& centers) const {
    int numLinks = links.size();
    vector<IdRealDist> lens(numLinks);
    for (int i = 0; i < numLinks; i++) {
        lens[i].id = i;
        lens[i].dist = metric.dSortValue(DPoint(links[i].i, links[i].j), centers[links[i].cId]);
    }
    sort(lens.begin(), lens.end());
    vector<Link> res(numLinks);
    for (int i = 0; i < numLinks; i++) {
        res[i] = links[lens[i].id];
    }
    links = move(res);
}





///////////////////////////////////////////////
////// misc stuff /////////////////////////////

void CircleGrower::step(vector<vector<int>>& plane, const vector<Point> &centers, int iter) {
    int k = centers.size();
    vector<int> quotas = Matching::centerQuotas(n, k, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (plane[i][j] != -1) {
                quotas[plane[i][j]]--;
            }
        }
    }
    Point p = P[iter];
    int i = p.i, j = p.j;
    int iTrans[8] = {i, i, -i, -i, j, j, -j, -j};
    int jTrans[8] = {j, -j, j, -j, i, -i, i, -i};
    for (int t  = 0; t < 8; t++) {
        for (int cId = 0; cId < k; cId++) {
            int si = iTrans[t] + centers[cId].i;
            int sj = jTrans[t] + centers[cId].j;
            if (si>=0 && sj>=0 && si<n && sj<n &&
                    plane[si][sj] == -1 && quotas[cId]>0) {
                plane[si][sj] = cId;
                quotas[cId]--;
            }
        }
    }
}

void CircleGrower::solveIntSiteCutoff(const vector<Point> &centers,
        Matching &M, int cutoff) const {

    int k = centers.size();
    vector<int> remCIds(0);
    remCIds.reserve(k);
    for (int i = 0; i < k; i++) {
        if (M.quotas[i] > 0) {
            remCIds.push_back(i);
        }
    }
    if (remCIds.empty()) return;

    int PIndex = 0;
    int numRemSites = n*n;
    while (PIndex < (int)P.size()) {
        const Point& p = P[PIndex];
        int i = p.i, j = p.j;
        int iTransform[8] = {i, i, -i, -i, j, j, -j, -j};
        int jTransform[8] = {j, -j, j, -j, i, -i, i, -i};
        for (int t  = 0; t < 8; t++) {
            for (int cId : remCIds) {
                int si = iTransform[t] + centers[cId].i;
                int sj = jTransform[t] + centers[cId].j;
                if (si>=0 && sj>=0 && si<n && sj<n &&
                        M.plane[si][sj] == -1 && M.quotas[cId]>0) {
                    M.plane[si][sj] = cId;
                    M.quotas[cId]--;
                    numRemSites--;
                    if (numRemSites == cutoff) return;
                }
            }
        }
        PIndex++;

        if (PIndex%64 == 0) { //update data structs from time to time
            vector<int> newCIds(0);
            newCIds.reserve(remCIds.size());
            for (int cId : remCIds) {
                if (M.quotas[cId] > 0) {
                    newCIds.push_back(cId);
                }
            }
            remCIds = move(newCIds);
        }
    }
}



double CircleGrower::idealAvgDistPointCenter(int regionSize) {
    double total = 0;
    int pInd = 0;
    int count = 0;
    while (count < regionSize) {
        Point p = P[pInd];
        int symPoints;
        if (p.i == 0 && p.j == 0) {
            symPoints = 1;
        } else if (p.j == 0 || p.i == p.j) {
            symPoints = 4;
        } else {
            symPoints = 8;
        }
        if (count + symPoints >= regionSize) {
            symPoints = regionSize - count;
        }
        total += symPoints*metric.ddist(p, Point(0, 0));
        count += symPoints;
        pInd++;
    }
    return total/regionSize;
}



