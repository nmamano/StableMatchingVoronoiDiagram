#include "siteheap.h"
#include "radixsort.h"
#include <unordered_map>

void SiteHeap::solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const
{

    int numPoints = 0;
    for (int quota : quotas) numPoints += quota;
    if (numPoints == 0) return;

    vector<int> remCIds = remainingCenterIds(quotas);
    vector<Point> points = remainingPoints(plane, numPoints);

    vector<int> remPointInds(numPoints);
    for (int i = 0; i < numPoints; i++) remPointInds[i] = i;

    vector<vector<SiteHeap::SiteCenter>> vps(numPoints);
    for (int i = 0; i < numPoints; i++) {
        vps[i] = centersSortedByDist(centers, points[i], i, remCIds);
    }

    vector<int> vpInds(numPoints, 0);
    vector<bool> matched(numPoints, false);

    SiteHeap::min_heap<SiteHeap::SiteCenter> Q = buildHeap(remPointInds, vps, vpInds);

    while (not Q.empty()) {
        SiteHeap::SiteCenter sc = Q.top();
        int cId = sc.cId;
        if (quotas[cId] == 0) {
            remPointInds = removeMatchedPoints(remPointInds, matched);
            advanceInds(quotas, remPointInds, vps, vpInds);
            Q = buildHeap(remPointInds, vps, vpInds);
        } else {
            Point site = points[sc.siteInd];
            plane[site.i][site.j] = cId;
            quotas[cId]--;
            matched[sc.siteInd] = true;
            Q.pop();
        }
    }
}

vector<SiteHeap::SiteCenter> SiteHeap::centersSortedByDist(const vector<Point>& centers,
        const Point& q, int qInd, const vector<int>& cIds) const {

    int numCenters = cIds.size();
    if (metric.hasIntDists()) {
        vector<int> dists(numCenters);
        for (int i = 0; i < numCenters; i++) {
            int dis = metric.iSortValue(q, centers[cIds[i]]);
            dists[i]=dis;
        }
        radix_sort(dists);
        unordered_map<int,int> dist2Index;
        for (int i = numCenters-1; i >= 0; i--) {
            //reverse loop because we want the first occurrence of each distance (?)
            dist2Index[dists[i]] = i;
        }
        vector<SiteHeap::SiteCenter> pairs(cIds.size());
        for (int i = 0; i < numCenters; i++) {
            int dis = metric.iSortValue(q, centers[cIds[i]]);
            int index = dist2Index[dis];
            pairs[index] = SiteHeap::SiteCenter(qInd, cIds[i], dis);
            dist2Index[dis] = dist2Index[dis]+1;
        }
        return pairs;
    } else {
        vector<SiteHeap::SiteCenter> pairs;
        pairs.reserve(numCenters);
        for (int i = 0; i < numCenters; i++) {
            double dis = metric.dSortValue(q, centers[cIds[i]]);
            pairs.push_back(SiteHeap::SiteCenter(qInd, cIds[i], dis));
        }
        sort(pairs.begin(), pairs.end());
        return pairs;
    }
}

vector<int> SiteHeap::removeMatchedPoints(const vector<int>& remPointInds, const vector<bool>& matched) {
    vector<int> res;
    int numRemPoints = remPointInds.size();
    res.reserve(numRemPoints);
    for (int i = 0; i < numRemPoints; i++) {
        int pInd = remPointInds[i];
        if (!matched[pInd]) {
            res.push_back(pInd);
        }
    }
    return res;
}

void SiteHeap::advanceInds(const vector<int>& quotas, const vector<int>& remPointInds,
        const vector<vector<SiteHeap::SiteCenter>>& vps, vector<int>& vpInds) {

    int numRemPoints = remPointInds.size();
    for (int i = 0; i < numRemPoints; i++) {
        int pInd = remPointInds[i];
        int cId = vps[pInd][vpInds[pInd]].cId;
        while (quotas[cId] == 0) {
            vpInds[pInd]++;
            cId = vps[pInd][vpInds[pInd]].cId;
        }
    }
}

SiteHeap::min_heap<SiteHeap::SiteCenter> SiteHeap::buildHeap(
        const vector<int>& remPointInds,
        const vector<vector<SiteHeap::SiteCenter>>& vps,
        const vector<int>& vpInds) {

    int numRemPoints = remPointInds.size();
    vector<SiteHeap::SiteCenter> pairs;
    pairs.reserve(numRemPoints);
    for (int i = 0; i < numRemPoints; i++) {
        int pInd = remPointInds[i];
        pairs.push_back(vps[pInd][vpInds[pInd]]);
    }
    SiteHeap::min_heap<SiteHeap::SiteCenter> Q(pairs.begin(), pairs.end());
    return Q;
}

//void SiteHeap::printDSsSH() {
//    cout<<endl<<"Quotas:"<<endl;
//    for(int q:quotas)cout<<q<<" ";
//    cout<<endl<<"matched:"<<endl;
//    for(bool b:matched)cout<<b<<" ";
//    cout<<endl<<"remPointInds:"<<endl;
//    for(int i:remPointInds)cout<<i<<" ";
//    cout<<endl<<"vpInds:"<<endl;
//    for(int i:vpInds)cout<<i<<" ";
//    cout<<endl<<"heapSize:"<<Q.size()<<endl;
//}

//    cout<<"Q:"<<endl;Q2 = Q;
//    while(not Q2.empty()) {
//        SiteHeap::SiteCenter sc = Q2.top();
//        cout << sc << " ";Q2.pop();}
//    cout << endl;
