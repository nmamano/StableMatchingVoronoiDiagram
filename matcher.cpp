#include "matcher.h"
#include <vector>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

using namespace std;

Matcher::Matcher(int n, Metric metric):
    n(n), metric(metric)
{
    buildP();
}

void Matcher::setN(int newN) {
    if (newN == n) return;
    n = newN;
    if ((int)P.size() < n*(n+1)/2) buildP();
}

void Matcher::setMetric(Metric newMetric) {
    if (newMetric == metric) return;
    metric = newMetric;
    buildP();
}

void Matcher::insertion_sort(vector<int> &array, int offset, int end) {
    int x, y, temp;
    for (x=offset; x<end; ++x) {
        for (y=x; y>offset && array[y-1]>array[y]; y--) {
            temp = array[y];
            array[y] = array[y-1];
            array[y-1] = temp;
        }
    }
}

void Matcher::radix_sort(vector<int>& array, int offset, int end, int shift) {
    int x, y, value, temp;
    int last[256] = { 0 }, pointer[256];

    for (x=offset; x<end; ++x) {
        ++last[(array[x] >> shift) & 0xFF];
    }

    last[0] += offset;
    pointer[0] = offset;
    for (x=1; x<256; ++x) {
        pointer[x] = last[x-1];
        last[x] += last[x-1];
    }

    for (x=0; x<256; ++x) {
        while (pointer[x] != last[x]) {
            value = array[pointer[x]];
            y = (value >> shift) & 0xFF;
            while (x != y) {
                temp = array[pointer[y]];
                array[pointer[y]++] = value;
                value = temp;
                y = (value >> shift) & 0xFF;
            }
            array[pointer[x]++] = value;
        }
    }

    if (shift > 0) {
        shift -= 8;
        for (x=0; x<256; ++x) {
            temp = x > 0 ? pointer[x] - pointer[x-1] : pointer[0] - offset;
            if (temp > 64) {
                radix_sort(array, pointer[x] - temp, pointer[x], shift);
            } else if (temp > 1) {
                insertion_sort(array, pointer[x] - temp, pointer[x]);
            }
        }
    }
}

void Matcher::radix_sort(vector<int>& array) {
    radix_sort(array, 0, array.size(), 24);
}

vector<Point> Matcher::pointsSortedByDistOrigin(int n, const Metric& metric) {
    if (metric.val==2) { //optimized case
        return pointsSortedByDistOriginL2(n);
    }
    return pointsSortedByDistOriginGenCase(n, metric);
}

vector<Point> Matcher::pointsSortedByDistOriginL2(int n) {
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

vector<Point> Matcher::pointsSortedByDistOriginGenCase(int n, const Metric& metric) {
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

vector<Matcher::Link> Matcher::linksSortedByLength(int n,
        const vector<Point>& centers, const vector<Point>& remPoints,
        const vector<int>& remCIds, const Metric& metric) {

    if (metric.val == 2) { //optimized case
        return linksSortedByLengthL2(n, centers, remPoints, remCIds);
    }
    return linksSortedByLengthGenCase(centers, remPoints, remCIds, metric);

}

vector<Matcher::Link> Matcher::linksSortedByLengthL2(int n, const vector<Point>& centers,
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

    vector<Matcher::Link> res(remCIds.size()*remPoints.size());
    for (const Point& p : remPoints) {
        for (int c_id : remCIds) {
            Point c = centers[c_id];
            int disSq = (p.i-c.i)*(p.i-c.i)+(p.j-c.j)*(p.j-c.j);
            res[counts[disSq]] = Matcher::Link(p.i, p.j, c_id);
            counts[disSq]++;
        }
    }
    return res;
}

vector<Matcher::Link> Matcher::linksSortedByLengthGenCase(
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
                int dis = metric.dSortValue(p, c);
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
        return res;
    }
}

void Matcher::buildP() {
    P = pointsSortedByDistOrigin(n, metric);
}

void Matcher::innerLoop(int pi, int pj,
        const vector<int>& c_ids, const vector<Point>& centers,
        vector<vector<int>>& plane, vector<int>& quotas) const {
    for (int c_id : c_ids) {
        int i = pi + centers[c_id].i;
        int j = pj + centers[c_id].j;
        if (i>=0 && j>=0 && i<n && j<n &&
                plane[i][j] == -1 && quotas[c_id]>0) {
            plane[i][j] = c_id;
            quotas[c_id]--;
        }
    }
}

void Matcher::middleLoop(int i, int j,
         const vector<int>& c_ids, const vector<Point>& centers,
         vector<vector<int>>& plane, vector<int>& quotas) const {
    innerLoop(i, j, c_ids, centers, plane, quotas);
    innerLoop(j, i, c_ids, centers, plane, quotas);
    innerLoop(i, -j, c_ids, centers, plane, quotas);
    innerLoop(j, -i, c_ids, centers, plane, quotas);
    innerLoop(-i, j, c_ids, centers, plane, quotas);
    innerLoop(-j, i, c_ids, centers, plane, quotas);
    innerLoop(-i, -j, c_ids, centers, plane, quotas);
    innerLoop(-j, -i, c_ids, centers, plane, quotas);
}

void Matcher::step(vector<vector<int>>& plane, const vector<Point> &centers, int iter) {
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
    middleLoop(p.i, p.j, c_ids, centers, plane, quotas);
}


void Matcher::incrementalMatching(const vector<Point> &centers,
        vector<vector<int>>& plane, vector<int>& quotas,
        double cutoff) const {

    int k = centers.size();

    plane = vector<vector<int>> (n, vector<int> (n, -1));
    quotas = centerQuotas(n, k);

    vector<int> rem_c_ids(k);
    for (int i = 0; i < k; i++) rem_c_ids[i] = i;

    int iter = 0;
    int inner_iter = 0;
    for (const Point& p : P) {
        middleLoop(p.i, p.j, rem_c_ids, centers, plane, quotas);
        iter++;
        inner_iter += 8*rem_c_ids.size();

        if (iter%64 == 0) { //update data structs from time to time
            vector<int> new_ids(0);
            new_ids.reserve(rem_c_ids.size());
            int num_rem_points = 0;
            for (int c_id : rem_c_ids) {
                if (quotas[c_id] > 0) {
                    new_ids.push_back(c_id);
                    num_rem_points += quotas[c_id];
                }
            }
            rem_c_ids = move(new_ids);

            int rem_centers = rem_c_ids.size();
            int rem_links = rem_centers * num_rem_points;
            if (num_rem_points == 0 || (float) rem_links/inner_iter < cutoff)
                return;
        }
    }
    return;
}

vector<vector<int>> Matcher::query(const vector<Point> &centers,
        double cutoff) const {
    vector<vector<int>> plane;
    vector<int> quotas;
    incrementalMatching(centers, plane, quotas, cutoff);
    allPairSortingMatching(centers, plane, quotas);
//    nearestNeighborMatching(centers, plane, quotas);
    return plane;
}






void Matcher::allPairSortingMatching(const vector<Point>& centers,
        vector<vector<int>>& plane, vector<int>& quotas) const {

    int num_rem_points = 0;
    for (int quota : quotas) num_rem_points += quota;
    if (num_rem_points == 0) return;

    vector<int> remCIds = remainingCenterIds(quotas);
    vector<Point> remPoints = remainingPoints(plane, num_rem_points);

    vector<Matcher::Link> L = linksSortedByLength(n, centers, remPoints, remCIds, metric);

    for (const Matcher::Link& link : L) {
        int i = link.i, j = link.j, c_id = link.c_id;
        if (quotas[c_id] > 0 and plane[i][j] == -1) {
            plane[i][j] = c_id;
            quotas[c_id]--;
            num_rem_points--;
            if (num_rem_points == 0) break;
        }
    }
    if (num_rem_points != 0) {
        cout << "Error: unasigned points" << endl;
    }
}

vector<int> Matcher::remainingCenterIds(const vector<int>& quotas) {
    int k = quotas.size();
    vector<int> res(0);
    for (int i = 0; i < k; i++) {
        if (quotas[i] > 0)
            res.push_back(i);
    }
    return res;
}

vector<Point> Matcher::remainingPoints(const vector<vector<int>>& plane, int num_rem_points) {
    int count = 0;
    int n = plane.size();
    vector<Point> res(num_rem_points);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (plane[i][j] == -1) {
                res[count] = Point(i,j);
                count++;
            }
        }
    }
    return res;
}






void Matcher::nearestNeighborMatching(const vector<Point>& centers,
        vector<vector<int>>& plane, vector<int>& quotas) const {

    int numNNQueries = 0;

    int numRemPoints = 0;
    for (int quota : quotas) numRemPoints += quota;
    if (numRemPoints == 0) return;

    vector<int> remCIds = remainingCenterIds(quotas);
    vector<Point> remPoints = remainingPoints(plane, numRemPoints);

    vector<int> closestSiteInd(centers.size());
    vector<vector<int>> closestCIds(remPoints.size());

    vector<bool> markedCenters(centers.size(), false);
    vector<double> dists(centers.size());

    for (int cId : remCIds) {
        int siteInd = findIndexClosest(centers[cId], remPoints);
        numNNQueries++;
        closestSiteInd[cId] = siteInd;
        closestCIds[siteInd].push_back(cId);
        dists[cId] = metric.dSortValue(centers[cId], remPoints[siteInd]);
    }

//    cout<<"begin"<<endl;
    int iter = 0;
    while (remCIds.size() > 0) {
//        cout<<"======================== ITER "<<iter<<endl;
//        cout<<"remaining points:"<<endl;
//        for (int i = 0; i < (int)remPoints.size();i++)
//            cout<<i<<":"<<remPoints[i]<<" ";
//        cout<<endl<< "remaining centers ids:"<<endl;
//        for (int i = 0; i < (int)remCIds.size(); i++)
//            cout<<remCIds[i]<<"("<<quotas[remCIds[i]]<<") ";
//        cout<<endl<<"neighbors:"<<endl;
//        for (int i = 0; i < (int)neighborInds.size();i++)
//            if(quotas[i]>0)
//                cout<<i<<":"<<neighborInds[i]<<" ";
//        cout<<endl<<"nearestCIds:"<<endl;
//        for (int j = 0; j < (int)nearestCIds.size(); j++){
//            if (nearestCIds[j].size() != 0) {
//                cout<<j<<":(";
//                for(int i:nearestCIds[j])cout<<i<<" ";
//                cout<<") ";
//            }
//        }
//        cout<<endl;

//        cout<<"find index of center of closest pair: ";
        int closestCIdInd = cIdIndexWithClosestNeighbor(remCIds, dists);
        int closestCId = remCIds[closestCIdInd];
//        cout <<closestInd<<" with cId "<<closestCId<<endl;
        int siteInd = closestSiteInd[closestCId];
        while (markedCenters[closestCId]) {
            markedCenters[closestCId] = false;
            siteInd = findIndexClosest(centers[closestCId], remPoints);
            numNNQueries++;
            closestSiteInd[closestCId] = siteInd;
            closestCIds[siteInd].push_back(closestCId);
            dists[closestCId] = metric.dSortValue(centers[closestCId], remPoints[siteInd]);

            closestCIdInd = cIdIndexWithClosestNeighbor(remCIds, dists);
            closestCId = remCIds[closestCIdInd];
            siteInd = closestSiteInd[closestCId];
        }

        Point site = remPoints[siteInd];
//        cout<<"index of site of closest pair: "<<siteInd<<" with site "<<site<<endl;

//        cout << "assign " << remPoints[siteInd] << " to " << closestCId << endl;
        plane[site.i][site.j] = closestCId;

//        cout<<"decrease quota to "<<quotas[closestCId]-1<<endl;
        quotas[closestCId]--;
        if (quotas[closestCId] == 0) {
//            cout<<"remove center"<<endl;
            remCIds[closestCIdInd] = remCIds[remCIds.size()-1];
            remCIds.pop_back();
        }

//        cout<<"save neighbors to site to be removed: "<<endl;
        vector<int> cIdsSite = closestCIds[siteInd];
//        for (int i = 0; i < (int)cIdsSite.size();i++)cout<<cIdsSite[i]<<" ";
//        cout<<endl;

//        cout<<"remove site "<<siteInd<<" "<<site<<" by moving point "<<remPoints.size()-1<<" ";
//        cout<<remPoints[remPoints.size()-1]<<" into its place and removing last place"<<endl;
        remPoints[siteInd] = remPoints[remPoints.size()-1];
        closestCIds[siteInd] = closestCIds[remPoints.size()-1];
        remPoints.pop_back();
        closestCIds.pop_back();

//        cout<<"update neighbors of centers pointing to moved site "<<siteInd<<endl;
        if (siteInd != (int)remPoints.size()) {
            for (int cId : closestCIds[siteInd]) {
//                cout<<cId<< "new neighbor "<<neighborInds[cId]<<"->";
                closestSiteInd[cId] = siteInd;
//                cout<<siteInd<<endl;
            }
        } else{
//            cout<<"nothing to do"<<endl;
        }
//        cout << "update neighbors of centers pointing to removed site"<<endl;
        for (int cId : cIdsSite) {
            if (quotas[cId] > 0) {
                markedCenters[cId] = true;
//                cout<<cId<<endl;
//                int newSiteInd = findIndexClosest(centers[cId], remPoints);
//                numNNQueries++;
//                closestSiteInd[cId] = newSiteInd;
//                closestCIds[newSiteInd].push_back(cId);
//                cout<<" new neighbor "<<newSiteInd<<endl;
            }
        }
        iter++;
    }
//    cout<<"end"<<endl;
    cout << "iter: " << iter << " num NN queries: " << numNNQueries << endl;
    if (remPoints.size() != 0) {
        cout <<"Error: unassigned points"<<endl;
    }
}


int Matcher::findIndexClosest(const Point& q, const vector<Point>& ps) const {
    int closest_ind = 0;
    double minDist = metric.dSortValue(q, ps[0]);
    for (int i = 1; i < (int) ps.size(); i++) {
        double dist = metric.dSortValue(q, ps[i]);
        if (dist < minDist) {
            closest_ind = i;
            minDist = dist;
        }
    }
    return closest_ind;
}

int Matcher::cIdIndexWithClosestNeighbor(const vector<Point>& centers,
        const vector<int>& remCIds, const vector<Point>& remPoints, const vector<int>& neighbors) const {

    int closestInd = 0;
    double minDist = metric.dSortValue(centers[remCIds[0]], remPoints[neighbors[remCIds[0]]]);
    for (int i = 1; i < (int) remCIds.size(); i++) {
        int cId = remCIds[i];
        double dist = metric.dSortValue(centers[cId], remPoints[neighbors[cId]]);
        if (dist < minDist) {
            minDist = dist;
            closestInd = i;
        }
    }
    return closestInd;
}

int Matcher::cIdIndexWithClosestNeighbor(const vector<int>& remCIds, const vector<double>& dists) const {

    int closestInd = 0;
    double minDist = dists[remCIds[closestInd]];
    for (int i = 1; i < (int) remCIds.size(); i++) {
        double dist = dists[remCIds[i]];
        if (dist < minDist) {
            closestInd = i;
            minDist = dist;
        }
    }
    return closestInd;
}















void Matcher::siteHeapMatching(const vector<Point>& centers,
        vector<vector<int>>& plane, vector<int>& quotas) const {

    int numPoints = 0;
    for (int quota : quotas) numPoints += quota;
    if (numPoints == 0) return;

    vector<int> remCIds = remainingCenterIds(quotas);
    vector<Point> points = remainingPoints(plane, numPoints);

    vector<int> remPointInds(numPoints);
    for (int i = 0; i < numPoints; i++) remPointInds[i] = i;

    vector<vector<Matcher::SiteCenter>> vps(numPoints);
    for (int i = 0; i < numPoints; i++) {
        vps[i] = centersSortedByDist(centers, points[i], i, remCIds);
    }

    vector<int> vpInds(numPoints, 0);
    vector<bool> matched(numPoints, false);

    Matcher::min_heap<Matcher::SiteCenter> Q = buildHeap(remPointInds, vps, vpInds);

//    cout<<"Q:"<<endl;
//    Matcher::min_heap<Matcher::SiteCenter> Q2 = Q;
//    while(not Q2.empty()) {
//        Matcher::SiteCenter sc = Q2.top();
//        cout << sc << " ";Q2.pop();}
//    cout << endl;

    while (not Q.empty()) {
        Matcher::SiteCenter sc = Q.top();
        int cId = sc.cId;
        if (quotas[cId] == 0) {
            remPointInds = removeMatchedPoints(remPointInds, matched);
            advanceInds(quotas, remPointInds, vps, vpInds);
            Q = buildHeap(remPointInds, vps, vpInds);
//            cout<<"Q:"<<endl;Q2 = Q;
//            while(not Q2.empty()) {
//                Matcher::SiteCenter sc = Q2.top();
//                cout << sc << " ";Q2.pop();}
//            cout << endl;
        } else {
            Point site = points[sc.siteInd];
            plane[site.i][site.j] = cId;
            quotas[cId]--;
            matched[sc.siteInd] = true;
            Q.pop();
        }
    }
//    cout<<"Q:"<<endl;Q2 = Q;
//    while(not Q2.empty()) {
//        Matcher::SiteCenter sc = Q2.top();
//        cout << sc << " ";Q2.pop();}
//    cout << endl;
}

vector<Matcher::SiteCenter> Matcher::centersSortedByDist(const vector<Point>& centers,
        const Point& q, int qInd, const vector<int>& cIds) const {

//    cout<<"cIds:";for(int i:cIds)cout<<i<<" ";cout<<endl;
    int numCenters = cIds.size();
    if (metric.hasIntDists()) {
        vector<int> dists(numCenters);
        for (int i = 0; i < numCenters; i++) {
            int dis = metric.iSortValue(q, centers[cIds[i]]);
            dists[i]=dis;
        }
//        cout<<"dists:";for(int d:dists)cout<<d<<" ";cout<<endl;
        radix_sort(dists);
//        cout<<"sorted dists:";for(int d:dists)cout<<d<<" ";cout<<endl;
        unordered_map<int,int> dist2Index;
        for (int i = numCenters-1; i >= 0; i--) {
            //reverse loop because we want the first occurrence of each distance (?)
            dist2Index[dists[i]] = i;
        }
        vector<Matcher::SiteCenter> pairs(cIds.size());
        for (int i = 0; i < numCenters; i++) {
            int dis = metric.iSortValue(q, centers[cIds[i]]);
            int index = dist2Index[dis];
            pairs[index] = Matcher::SiteCenter(qInd, cIds[i], dis);
            dist2Index[dis] = dist2Index[dis]+1;
        }
//        cout<<"bad pairs:"<<endl;for(Matcher::SiteCenter sc:pairs)cout<<sc<<" ";cout<<endl;
//        {
//            vector<Matcher::SiteCenter> pairs;
//            pairs.reserve(numCenters);
//            for (int i = 0; i < numCenters; i++) {
//                double dis = metric.dSortValue(q, centers[cIds[i]]);
//                pairs.push_back(Matcher::SiteCenter(qInd, cIds[i], dis));
//            }
//            sort(pairs.begin(), pairs.end());
//            cout<<"good pairs:"<<endl;for(Matcher::SiteCenter sc:pairs)cout<<sc<<" ";cout<<endl;

//        }
        return pairs;
    } else {
        vector<Matcher::SiteCenter> pairs;
        pairs.reserve(numCenters);
        for (int i = 0; i < numCenters; i++) {
            double dis = metric.dSortValue(q, centers[cIds[i]]);
            pairs.push_back(Matcher::SiteCenter(qInd, cIds[i], dis));
        }
        sort(pairs.begin(), pairs.end());
//        cout<<"pairs:"<<endl;for(Matcher::SiteCenter sc:pairs)cout<<sc<<" ";cout<<endl;
        return pairs;
    }
}

vector<int> Matcher::removeMatchedPoints(const vector<int>& remPointInds, const vector<bool>& matched) {
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

void Matcher::advanceInds(const vector<int>& quotas, const vector<int>& remPointInds,
        const vector<vector<Matcher::SiteCenter>>& vps, vector<int>& vpInds) {

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

Matcher::min_heap<Matcher::SiteCenter> Matcher::buildHeap(
        const vector<int>& remPointInds,
        const vector<vector<Matcher::SiteCenter>>& vps,
        const vector<int>& vpInds) {

    int numRemPoints = remPointInds.size();
    vector<Matcher::SiteCenter> pairs;
    pairs.reserve(numRemPoints);
    for (int i = 0; i < numRemPoints; i++) {
        int pInd = remPointInds[i];
        pairs.push_back(vps[pInd][vpInds[pInd]]);
    }
    Matcher::min_heap<Matcher::SiteCenter> Q(pairs.begin(), pairs.end());
    return Q;
}

//void Matcher::printDSsSH() {
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






