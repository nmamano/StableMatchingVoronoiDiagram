#include "matcher.h"
#include "matchingutils.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <iostream>
using namespace std;

vector<vector<int>> Matcher::assignPoints(int n, const vector<Point>& centers) {
    int k = centers.size();

    vector<Link> L = sortedLinks(n,  centers);
    vector<vector<int>> plane(n, vector<int> (n, -1));

    //indicates how many sites each center still needs to complete its region
    vector<int> quotas = centerQuotas(n, k);

    int num_assigned_points = 0;
    for (const Link link : L) {
        int i = link.i, j = link.j;
        int c_id = link.c_id;
        if (quotas[c_id] > 0 and plane[i][j] == -1) {
            plane[i][j] = c_id;
            quotas[c_id]--;
            num_assigned_points++;
            if (num_assigned_points == n*n) break;
        }
    }
    if (num_assigned_points != n*n) {
        cerr << "Error: unasigned points" << endl;
    }
    return plane;
}

vector<Link> Matcher::sortedLinks(int n, const vector<Point>& centers) {
    int k = centers.size();
    vector<Link> L(n*n*k);
    int index = 0;
    for (int c_id = 0; c_id < k; c_id++) {
        int ci = centers[c_id].i;
        int cj = centers[c_id].j;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                float dis_squared = (i-ci)*(i-ci)+(j-cj)*(j-cj);
                L[index] = Link(i, j, c_id, dis_squared);
                index++;
            }
        }
    }
    sort(L.begin(), L.end());
    return L;
}
