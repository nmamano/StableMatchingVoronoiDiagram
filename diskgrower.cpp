#include "diskgrower.h"
#include "radixsort.h"
#include "allpairsorting.h"
#include "matchingutils.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

DiskGrower::DiskGrower(int n, Metric metric):
    n(n), metric(metric)
{
    bimatcher = new AllPairSorting(metric);
    buildP();
}

void DiskGrower::setN(int newN) {
    if (newN == n) return;
    n = newN;
    if ((int)P.size() < n*(n+1)/2) buildP();
}

void DiskGrower::setMetric(Metric newMetric) {
    if (newMetric == metric) return;
    metric = newMetric;
    buildP();
}

vector<Point> DiskGrower::pointsSortedByDistOrigin(int n, const Metric& metric) {
    if (metric.val==2) { //optimized case
        return pointsSortedByDistOriginL2(n);
    }
    return pointsSortedByDistOriginGenCase(n, metric);
}

vector<Point> DiskGrower::pointsSortedByDistOriginL2(int n) {
    //counting sort
    vector<int> counts(n*n*2, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            int disSq = i*i+j*j;
            counts[disSq]++;
        }
    }
    int total = 0;
    for (int i = 0; i < n*n*2; i++) {
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

vector<Point> DiskGrower::pointsSortedByDistOriginGenCase(int n, const Metric& metric) {
    if (metric.hasIntDists()) { //integer sorting
        vector<int> dists(n*(n+1)/2);
        int index = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                dists[index] = metric.iSortValue(Point(i, j), Point(0, 0));
                index++;
            }
        }
        radix_sort(dists);
        unordered_map<int,int> dist2Index;
        for (int i = n*(n+1)/2-1; i >= 0; i--) {
            //reverse loop because we want the first occurrence of each distance (?)
            dist2Index[dists[i]] = i;
        }
        vector<Point> res(n*(n+1)/2);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                int dis = metric.iSortValue(Point(i, j), Point(0, 0));
                int index = dist2Index[dis];
                res[index] = Point(i, j);
                dist2Index[dis] = dist2Index[dis]+1;
            }
        }
        return res;
    } else {
        vector<double> dists(n*(n+1)/2);
        int index = 0;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                dists[index] = metric.dSortValue(Point(i, j), Point(0, 0));
                index++;
            }
        }
        sort(dists.begin(), dists.end());
        unordered_map<double,int> dist2Index;
        for (int i = n*(n+1)/2-1; i >= 0; i--) {
            //reverse loop because we want the first occurrence of each distance
            dist2Index[dists[i]] = i;
        }
        vector<Point> res(n*(n+1)/2);
        for (int i = 0; i < n; i++) {
            for (int j = 0; j <= i; j++) {
                double dis = metric.dSortValue(Point(i, j), Point(0, 0));
                int index = dist2Index[dis];
                res[index] = Point(i, j);
                dist2Index[dis] = dist2Index[dis]+1;
            }
        }
        return res;
    }
}

void DiskGrower::buildP() {
    P = pointsSortedByDistOrigin(n, metric);
}

void DiskGrower::processGridPoint(int pi, int pj,
        const vector<int>& remCIds, const vector<Point>& centers,
        vector<vector<int>>& plane, vector<int>& quotas) const {
    for (int cId : remCIds) {
        int i = pi + centers[cId].i;
        int j = pj + centers[cId].j;
        if (i>=0 && j>=0 && i<n && j<n &&
                plane[i][j] == -1 && quotas[cId]>0) {
            plane[i][j] = cId;
            quotas[cId]--;
        }
    }
}

void DiskGrower::processPPoint(int i, int j,
         const vector<int>& remCIds, const vector<Point>& centers,
         vector<vector<int>>& plane, vector<int>& quotas) const {
    processGridPoint(i, j, remCIds, centers, plane, quotas);
    processGridPoint(j, i, remCIds, centers, plane, quotas);
    processGridPoint(i, -j, remCIds, centers, plane, quotas);
    processGridPoint(j, -i, remCIds, centers, plane, quotas);
    processGridPoint(-i, j, remCIds, centers, plane, quotas);
    processGridPoint(-j, i, remCIds, centers, plane, quotas);
    processGridPoint(-i, -j, remCIds, centers, plane, quotas);
    processGridPoint(-j, -i, remCIds, centers, plane, quotas);
}

void DiskGrower::step(vector<vector<int>>& plane, const vector<Point> &centers, int iter) {
    int k = centers.size();
    vector<int> quotas = centerQuotas(n, k);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (plane[i][j] != -1) {
                quotas[plane[i][j]]--;
            }
        }
    }
    vector<int> c_ids(k);
    for (int i = 0; i < k; i++) c_ids[i] = i;
    Point p = P[iter];
    processPPoint(p.i, p.j, c_ids, centers, plane, quotas);
}


void DiskGrower::incrementalMatching(const vector<Point> &centers,
        vector<vector<int>>& plane, vector<int>& quotas,
        double cutoff, int& PIndex) const {

    int k = centers.size();
    vector<int> remCIds(0);
    remCIds.reserve(k);
    for (int i = 0; i < k; i++) {
        if (quotas[i] > 0) {
            remCIds.push_back(i);
        }
    }
    if (remCIds.empty()) return;

    int innerIter = 0;
    while (PIndex < (int)P.size()) {
        const Point& p = P[PIndex];
        processPPoint(p.i, p.j, remCIds, centers, plane, quotas);
        PIndex++;
        innerIter += 8*remCIds.size();

        if (PIndex%64 == 0) { //update data structs from time to time
            vector<int> newCIds(0);
            newCIds.reserve(remCIds.size());
            int numRemPoints = 0;
            for (int c_id : remCIds) {
                if (quotas[c_id] > 0) {
                    newCIds.push_back(c_id);
                    numRemPoints += quotas[c_id];
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

vector<vector<int>> DiskGrower::intCentersQuery(const vector<Point> &centers,
        double cutoff) const {
    vector<vector<int>> plane;
    vector<int> quotas;
    int k = centers.size();
    initEmpty(plane, quotas, k);
    int PIndex = 0;
    incrementalMatching(centers, plane, quotas, cutoff, PIndex);
    bimatcher->solve(centers, plane, quotas);
    return plane;
}

void DiskGrower::initEmpty(vector<vector<int>>& plane, vector<int>& quotas, int k) const {
    plane = vector<vector<int>> (n, vector<int> (n, -1));
    quotas = centerQuotas(n, k);
}

vector<vector<int>> DiskGrower::realCentersQuery(const vector<DPoint>& centers) const {
    vector<vector<int>> plane;
    vector<int> quotas;
    int k = centers.size();
    initEmpty(plane, quotas, k);

    vector<int> remCIds(k);
    for (int i = 0; i < k; i++) remCIds[i] = i;

//    cout<<"init cps and maxCpDist"<<endl;
    vector<Point> cps(k);
    double maxCpDist = 0;
    for (int i = 0; i < k; i++) {
        DPoint center = centers[i];
        cps[i] = Point(round(center.i), round(center.j));
        double d = metric.ddist(centers[i], cps[i]);
        if (d > maxCpDist) maxCpDist = d;
    }
//    cout<<"centers and cps"<<endl;
//    for(auto p:centers)cout<<p<<" ";cout<<endl;
//    for(auto p:cps)cout<<p<<" ";cout<<endl;

    int numPoints = P.size();
    int chunkStart = 0;
    while (remCIds.size() > 0) {
//        cout<<"numPoints:"<<numPoints<<endl;
//        cout<<"maxCpDist:"<<maxCpDist<<endl;
//        cout<<"chunkStart:"<<chunkStart<<endl;
        int chunkEnd = min(chunkStart+n, numPoints);
        if (chunkEnd == chunkStart) return plane;
        double disLast = metric.ddist(P[chunkEnd-1], Point(0, 0));
        double disThreshold = disLast + 2*maxCpDist;

        int extrasEnd = chunkEnd;
        while (extrasEnd < numPoints &&
               metric.ddist(P[extrasEnd], Point(0, 0)) < disThreshold) {
            extrasEnd++;
        }
//        cout<<"chunkEnd:"<<chunkEnd<<endl;
//        cout<<"extrasEnd:"<<extrasEnd<<endl;

        //generate all the links in the chunk
        vector<Link> chunkLinks;
        //reserve maximum space we could need
        chunkLinks.reserve(8*(extrasEnd-chunkStart)*remCIds.size());
        double lengthThreshold = disLast + maxCpDist;
//        cout<<"add links"<<endl;
        for (int i = chunkStart; i < chunkEnd; i++) {
            //augment threshold to make sure every point is included despite precission errors
            addLinksPPoint(P[i].i, P[i].j, remCIds, plane, centers, cps, chunkLinks, lengthThreshold+1);
        }
        for (int i = chunkEnd; i < extrasEnd; i++) {
            addLinksPPoint(P[i].i, P[i].j, remCIds, plane, centers, cps, chunkLinks, lengthThreshold);
        }
//        cout<<"sort links"<<endl;
        sortLinks(chunkLinks, centers);

//        cout<<"process links in order"<<endl;
        //process links in order
        for (const Link& link : chunkLinks) {
            int i = link.i, j = link.j, cId = link.cId;
            if (quotas[cId] > 0 and plane[i][j] == -1) {
                plane[i][j] = cId;
                quotas[cId]--;
            }
        }

//        cout<<"update remaining centers and maxCpDist"<<endl;
        //update remaining centers and maxCpDist
        maxCpDist = 0;
        vector<int> newCIds(0);
        newCIds.reserve(remCIds.size());
        for (int cId : remCIds) {
            if (quotas[cId] > 0) {
                newCIds.push_back(cId);
                double d = metric.ddist(centers[cId], cps[cId]);
                if (d > maxCpDist) maxCpDist = d;
            }
        }
        remCIds = move(newCIds);

//        cout<<"remCIds:"<<endl;for(int c:remCIds)cout<<c<<" ";cout<<endl;
//        cout<<"quotas:"<<endl;for(int q:quotas)cout<<q<<" ";cout<<endl;
//        cout<<"move to next chunk"<<endl;
        //move to next chunk
        chunkStart = chunkEnd;
    }
    return plane;
}

void DiskGrower::addLinksPPoint(int i, int j, const vector<int>& cIds,
    const vector<vector<int>>& plane, const vector<DPoint>& centers,
    const vector<Point>& cps, vector<Link>& links, double lengthThreshold) const {
    addLinksGridPoint( i,  j, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint( j,  i, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint( i, -j, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint( j, -i, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint(-i,  j, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint(-j,  i, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint(-i, -j, cIds, plane, centers, cps, links, lengthThreshold);
    addLinksGridPoint(-j, -i, cIds, plane, centers, cps, links, lengthThreshold);
}

void DiskGrower::addLinksGridPoint(int pi, int pj, const vector<int>& cIds,
    const vector<vector<int>>& plane, const vector<DPoint>& centers,
    const vector<Point>& cps, vector<Link>& links, double lengthThreshold) const {

    for (int cId : cIds) {
        int i = pi + cps[cId].i;
        int j = pj + cps[cId].j;
        if (i>=0 && j>=0 && i<n && j<n &&
                plane[i][j] == -1) {
            double length = metric.ddist(Point(i, j), centers[cId]);
            //add links only under the maximum safe link length
            if (length <= lengthThreshold) {
                links.push_back(Link(i, j, cId));
            }
        }
    }
}

void DiskGrower::sortLinks(vector<Link>& links, const vector<DPoint>& centers) const {
    int numLinks = links.size();
    vector<LengthIndex> lens(numLinks);
    for (int i = 0; i < numLinks; i++) {
        lens[i].index = i;
        lens[i].length = metric.dSortValue(Point(links[i].i, links[i].j), centers[links[i].cId]);
    }
    sort(lens.begin(), lens.end());
    vector<Link> res(numLinks);
    for (int i = 0; i < numLinks; i++) {
        res[i] = links[lens[i].index];
    }
    links = move(res);
}


















