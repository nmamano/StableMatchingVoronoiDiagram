#include "pairheapgen.h"
#include <unordered_map>
#include "radixsort.h"
#include "linearsearch.h"
#include <string>

string PairHeapGen::acronym() const {
    return string("PH_")+(lazy?"L":"E")+"_"+CPName;
}

void PairHeapGen::solveInt(const vector<Point> &centers, Matching& M) {
    if (M.remainingCIds().size() == 0) return;
    if (metric.hasIntDists()) {
        if (CPName == "LS") {
            //cerr<<'b';
            CP = new LinearSearch(metric, centers, M.remainingCIds());
            //cerr<<'c';
        } else {
            throw new runtime_error("CP method not found");
        }
    } else {
        vector<DPoint> realCenters(centers.begin(), centers.end());
        if (CPName == "LS") {
            CP = new LinearSearch(metric, realCenters, M.remainingCIds());
        } else {
            throw new runtime_error("CP method not found");
        }
    }
    //cerr<<'d';
    solve(M);
    //cerr<<'e';
}

void PairHeapGen::solveReal(const vector<DPoint> &centers, Matching& M) {
    if (M.remainingCIds().size() == 0) return;
    if (CPName == "LS") {
        //cerr<<'o';
        CP = new LinearSearch(metric, centers, M.remainingCIds());
        //cerr<<'p';
    } else {
        throw new runtime_error("CP method not found");
    }
    //cerr<<'q';
    solve(M);
    //cerr<<'r';
}

void PairHeapGen::solve(Matching& M) const {
    //cerr<<'f';
    vector<SiteStructGen> sites = initSites(M);
    //cerr<<'g';
    min_heap<RealDistIdPair> Q = initHeap(sites);
    //cerr<<'h';
    if (lazy) {
        //cerr<<'i';
        solveLazy(M, sites, Q);
        //cerr<<'j';
    } else {
        solveEager(M, sites, Q);
    }
}

vector<SiteStructGen> PairHeapGen::initSites(const Matching& M) const {
    vector<Point> remSites = M.remainingSites();
    int numSites = remSites.size();
    vector<SiteStructGen> sites(numSites);
    for (int sId = 0; sId < numSites; sId++) {
        sites[sId].site = remSites[sId];
        sites[sId].neighbor = CP->closestInd(remSites[sId]);
    }
    return sites;
}

min_heap<RealDistIdPair> PairHeapGen::initHeap(const vector<SiteStructGen> &sites) const {
    vector<RealDistIdPair> siteDists(0);
    siteDists.reserve(sites.size());

    for (int sId = 0; sId < (int)sites.size(); sId++) {
        double dist = CP->dist(sites[sId].site, sites[sId].neighbor);
        siteDists.push_back(RealDistIdPair(dist, sId));
    }
    return min_heap<RealDistIdPair> (siteDists.begin(), siteDists.end());
}

void PairHeapGen::solveLazy(Matching& M, vector<SiteStructGen>& sites, min_heap<RealDistIdPair>& Q) const {
    //cerr<<'k';
    while (!Q.empty()) {
        int sId = Q.top().id;
        Q.pop();
        int cId = sites[sId].neighbor;
        if (M.quotas[cId] == 0) {
            CP->delInd(cId);
            sites[sId].neighbor = CP->closestInd(sites[sId].site);
            double newDist = CP->dist(sites[sId].site, sites[sId].neighbor);
            Q.push(RealDistIdPair(newDist, sId));
        } else {
            Point site = sites[sId].site;
            M.plane[site.i][site.j] = cId;
            M.quotas[cId]--;
        }
    }
}

void PairHeapGen::solveEager(Matching& M, vector<SiteStructGen>& sites, min_heap<RealDistIdPair>& Q) const {
    DelSet remSIds(M.numRemSites());

    while (!Q.empty()) {
        int sId = Q.top().id;
        Q.pop();
        int cId = sites[sId].neighbor;
        if (M.quotas[cId] == 0) {
            CP->delInd(cId);
            Q = rebuildHeap(sites, M.quotas, remSIds.elems);
        } else {
            remSIds.erase(sId);
            Point site = sites[sId].site;
            M.plane[site.i][site.j] = cId;
            M.quotas[cId]--;
        }
    }
}

min_heap<RealDistIdPair> PairHeapGen::rebuildHeap(vector<SiteStructGen>& sites, const vector<int>& quotas,
        const vector<int>& sIds) const {

    vector<RealDistIdPair> siteDists(0);
    siteDists.reserve(sIds.size());

    for (int sId : sIds) {
        if (quotas[sites[sId].neighbor] == 0) {
            sites[sId].neighbor = CP->closestInd(sites[sId].site);
        }
        double dist = CP->dist(sites[sId].site, sites[sId].neighbor);
        siteDists.push_back(RealDistIdPair(dist, sId));
    }
    return min_heap<RealDistIdPair>(siteDists.begin(), siteDists.end());
}

