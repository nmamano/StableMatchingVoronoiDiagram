#include "pairheap.h"
#include <unordered_map>
#include "radixsort.h"

string PairHeap::acronym() const {
    if (lazy && presort) return "PHLP";
    if (lazy && !presort) return "PHLL";
    if (!lazy && presort) return "PHEP";
    return "PHEL";
}

void PairHeap::solveInt(const vector<Point> &centers, Matching& M) {
    if (metric.hasIntDists()) {
        intCenters = centers;
        real = false;
    } else {
        realCenters = vector<DPoint>(centers.begin(), centers.end());
        real = true;
    }
    solve(M);
}

void PairHeap::solveReal(const vector<DPoint> &centers, Matching& M) {
    realCenters = centers;
    real = true;
    solve(M);
}

void PairHeap::solve(Matching& M) const {
    DelSet remCIds(M.remainingCIds());
    if (remCIds.size() == 0) return;
    DelSet remSIds;
    int numSites = M.numRemSites();
    if (!lazy) remSIds = DelSet(numSites);

    vector<SiteStruct> sites = initSites(remCIds.elems, M);
    min_heap<IdRealDist> Q = initHeap(sites);
    while (!Q.empty()) {
        int sId = Q.top().id;
        Q.pop();
        int cId = sites[sId].neighbor;
        if (M.quotas[cId] == 0) {
            remCIds.erase(cId);
            if (lazy) {
                reinsertPair(sId, Q, sites, M.quotas, remCIds.elems);
            } else {
                Q = rebuildHeap(sites, M.quotas, remSIds.elems, remCIds.elems);
            }
        } else {
            Point site = sites[sId].site;
            M.plane[site.i][site.j] = cId;
            M.quotas[cId]--;
            if (!lazy) remSIds.erase(sId);
        }
    }
}

vector<int> PairHeap::centersSortedByDist(Point site, const vector<int>& cIds) const {
    int k = cIds.size();
    vector<int> res(k);
    if (!real) {
        vector<IdIntDist> idDists = centersSortedByIntDist(site, cIds);
        for (int i = 0; i < k; i++) res[i] = idDists[i].id;
    } else {
        vector<IdRealDist> idDists = centersSortedByRealDist(site, cIds);
        for (int i = 0; i < k; i++) res[i] = idDists[i].id;
    }
    return res;
}

vector<IdIntDist> PairHeap::centersSortedByIntDist(Point site, const vector<int>& cIds) const {

    int numCenters = cIds.size();
    vector<IdIntDist> pairs(numCenters);
    for (int i = 0; i < numCenters; i++) {
        pairs[i].id = cIds[i];
        pairs[i].dist = intDist(site, cIds[i]);
    }
    radix_sort(pairs);
    return pairs;
}

vector<IdRealDist> PairHeap::centersSortedByRealDist(Point site, const vector<int>& cIds) const {

    int numCenters = cIds.size();
    vector<IdRealDist> pairs(numCenters);
    for (int i = 0; i < numCenters; i++) {
        pairs[i].id = cIds[i];
        pairs[i].dist = realDist(site, cIds[i]);
    }
    sort(pairs.begin(), pairs.end());
    return pairs;
}

vector<SiteStruct> PairHeap::initSites(const vector<int>& cIds, const Matching& M) const {
    vector<Point> remSites = M.remainingSites();
    int numSites = remSites.size();
    vector<SiteStruct> sites(numSites);
    for (int sId = 0; sId < numSites; sId++) {
        sites[sId].site = remSites[sId];
        if (presort) {
            sites[sId].arrayIndex = 0;
            sites[sId].presortedArray = centersSortedByDist(sites[sId].site, cIds);
            sites[sId].neighbor = sites[sId].presortedArray[0];
        } else {
            sites[sId].neighbor = nearestCenter(sites[sId].site, cIds);
        }
    }
    return sites;
}

min_heap<IdRealDist> PairHeap::initHeap(const vector<SiteStruct> &sites) const {
    vector<IdRealDist> siteDists(sites.size());
    for (int sId = 0; sId < (int)sites.size(); sId++) {
        siteDists[sId].id = sId;
        siteDists[sId].dist = neighborDist(sId, sites);
    }
    return min_heap<IdRealDist> (siteDists.begin(), siteDists.end());
}

void PairHeap::reinsertPair(int sId, min_heap<IdRealDist>& Q, vector<SiteStruct>& sites,
        const vector<int>& quotas, const vector<int>& cIds) const {
    updateNeighbor(sId, sites, quotas, cIds);
    IdRealDist sd;
    sd.id = sId;
    sd.dist = neighborDist(sId, sites);
    Q.push(sd);
}

min_heap<IdRealDist> PairHeap::rebuildHeap(vector<SiteStruct>& sites, const vector<int>& quotas,
        const vector<int>& sIds, const vector<int>& cIds) const {

    vector<IdRealDist> siteDists(sIds.size());
    int i = 0;
    for (int sId : sIds) {
        if (quotas[sites[sId].neighbor] == 0) {
            updateNeighbor(sId, sites, quotas, cIds);
        }
        siteDists[i].id = sId;
        siteDists[i].dist = neighborDist(sId, sites);
        i++;
    }
    return min_heap<IdRealDist>(siteDists.begin(), siteDists.end());
}

void PairHeap::updateNeighbor(int sId, vector<SiteStruct>& sites,
        const vector<int>& quotas,
        const vector<int>& cIds) const {

    if (presort) {
        while (quotas[sites[sId].presortedArray[sites[sId].arrayIndex]] == 0) {
            sites[sId].arrayIndex++;
        }
        sites[sId].neighbor = sites[sId].presortedArray[sites[sId].arrayIndex];
    } else {
        sites[sId].neighbor = nearestCenter(sites[sId].site, cIds);
    }
}

double PairHeap::neighborDist(int sId, const vector<SiteStruct>& sites) const {
    Point site = sites[sId].site;
    int cId = sites[sId].neighbor;
    if (real) return realDist(site, cId);
    return intDist(site, cId);
}

int PairHeap::intDist(const Point& site, int cId) const {
    return metric.iSortValue(site, intCenters[cId]);
}

double PairHeap::realDist(const Point& site, int cId) const {
    return metric.dSortValue(site, realCenters[cId]);
}

int PairHeap::nearestCenter(const Point& p, const vector<int>& cIds) const {
    int res = cIds[0];
    if (real) {
        double minDist = realDist(p, cIds[0]);
        for (int i = 1; i < (int)cIds.size(); i++) {
            double dist = realDist(p, cIds[i]);
            if (dist < minDist) {
                res = cIds[i];
                minDist = dist;
            }
        }
    } else {
        int minDist = intDist(p, cIds[0]);
        for (int i = 1; i < (int)cIds.size(); i++) {
            int dist = intDist(p, cIds[i]);
            if (dist < minDist) {
                res = cIds[i];
                minDist = dist;
            }
        }
    }
    return res;
}



