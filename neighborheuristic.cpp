#include "neighborheuristic.h"

void NeighborHeuristic::solve(const vector<Point> &centers, vector<vector<int> > &plane, vector<int> &quotas) const
{

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
//    cout << "iter: " << iter << " num NN queries: " << numNNQueries << endl;
    if (remPoints.size() != 0) {
        cout <<"Error: unassigned points"<<endl;
    }
}


int NeighborHeuristic::findIndexClosest(const Point& q, const vector<Point>& ps) const {
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

int NeighborHeuristic::cIdIndexWithClosestNeighbor(const vector<Point>& centers,
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

int NeighborHeuristic::cIdIndexWithClosestNeighbor(const vector<int>& remCIds, const vector<double>& dists) const {

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


