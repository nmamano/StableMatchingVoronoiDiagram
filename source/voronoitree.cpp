#include "voronoitree.h"
#include <unordered_map>
#include <map>
#include <iostream>
#include <string>
#include "galeshapley.h"

using namespace std;

vector<vector<int>> VoronoiTree::getDiagram(const vector<NPoint>& centers, int n) {
    int k = centers.size();

    vector<string> sigs; //face signatures indexed by face id
    map<string, int> sigToId; //signature to face id map
    vector<vector<int> > diagram(n, vector<int> (n, -1));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            NPoint p(i, j);
            vector<double> dists(k);
            for (int cId = 0; cId < k; cId++) {
                dists[cId] = metric.ddist(p, centers[cId]);
            }
            vector<int> centerOrder = sortedRanks(dists);
            string sig(k, 255);
            for (int cId = 0; cId < k; cId++) {
                sig[cId] = centerOrder[cId];
            }
            if (sigToId.count(sig) == 0) {
                sigs.push_back(sig);
                sigToId[sig] = sigs.size()-1;
            }
            diagram[i][j] = sigToId.at(sig);
        }
    }
    F = sigs.size();
    cout << "OONVD has "<<F<<" faces"<<endl;
    cout << "should have "<<(k*(k-1)/2)*(k*(k-1)/2+1)/2+1-k*(k-2)*(k-1)/6<<endl;
    return diagram;
}

vector<int> VoronoiTree::getQuotas(int k) const {
    vector<int> quotas(k, F/k);
    int remQuotas = F%k;
    while (remQuotas > 0) {
        quotas[remQuotas-1]++;
        remQuotas--;
    }
    return quotas;
}

vector<vector<int>> VoronoiTree::solve(const vector<NPoint> &centers, int n)
{
    int k = centers.size();
    cerr<<1<<endl;
    vector<vector<int> > diagram = getDiagram(centers, n);
cerr<<2<<endl;
    vector<vector<double>> faceDists (F, vector<double> (k, -1)); //dists from closest point in face to centers
    vector<vector<double>> centerDists (k, vector<double> (F, -1)); //dists from centers to closest piont in face
//    vector<int> faceSizes(F, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int fId = diagram[i][j];
//            faceSizes[fId]++;
            NPoint p(i, j);
            for (int cId = 0; cId < k; cId++) {
                double d = metric.ddist(p, centers[cId]);
                if (d < faceDists[fId][cId] || faceDists[fId][cId] == -1) {
                    faceDists[fId][cId] = centerDists[cId][fId] = d;
                }
            }
        }
    }
cerr<<3<<endl;
    vector<vector<int>> centerPrefs(k, vector<int> (F));
    for (int i = 0; i < k; i++) {
        //cerr<<"centerDists "<<i<<": ";
        //for (int j=0;j<F;j++)cerr<<centerDists[i][j]<<" ";cerr<<endl;
        centerPrefs[i] = sortedRanks(centerDists[i]);
        //cerr<<"centerPrefs "<<i<<": ";
        //for (int j=0;j<F;j++)cerr<<centerPrefs[i][j]<<" ";cerr<<endl;
    }
cerr<<4<<endl;
    vector<int> quotas = getQuotas(k);
cerr<<5<<endl;
    GaleShapley gs;
    vector<int> matching = gs.solve(faceDists, centerPrefs, quotas);
cerr<<6<<endl;
    vector<vector<int>> plane(n, vector<int> (n, -1));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            plane[i][j] = matching[diagram[i][j]];
        }
    }
cerr<<7<<endl;
    return plane;
//    return diagram;
}
