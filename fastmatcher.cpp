#include "fastmatcher.h"
#include "matchingutils.h"
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

FastMatcher::FastMatcher(int n): n(n)
{
    earlyCutoff = -1;
    buildP();
}

void FastMatcher::buildP() {
    P = vector<Point>(n*n);
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            P[index] = Point(i,j);
            index++;
        }
    }
    sort(P.begin(), P.end());
}

void FastMatcher::updateP(int newN)
{
    n = newN;
    earlyCutoff = -1;
    if (P.size() < n*n) buildP();
}

vector<vector<int>> FastMatcher::query(const vector<Point> &centers)
{
    int k = centers.size();
    bool autoset_early_cutoff = earlyCutoff == -1;
    if (autoset_early_cutoff) setEarlyCutoff(600*cbrt(n*n*k));

    vector<vector<int>> plane(n, vector<int> (n, -1));
    vector<int> quotas = center_quotas(n, k);

    vector<int> remaining_center_ids(k);
    for (int i = 0; i < k; i++) remaining_center_ids[i] = i;

    const int itransform[4] = {1, -1, 1, -1};
    const int jtransform[4] = {1, 1, -1, -1};

    int iter = 0;
    for (const Point& p :P) {
        for (int r = 0; r < 4; r++) {
            int ii = p.i * itransform[r];
            int jj = p.j * jtransform[r];
            for (int c_id : remaining_center_ids) {
                int i = ii + centers[c_id].i;
                int j = jj + centers[c_id].j;
                if (i>=0 && j>=0 && i<n && j<n &&
                        plane[i][j] == -1 && quotas[c_id]>0) {
                    plane[i][j] = c_id;
                    quotas[c_id]--;
                }
            }
        }
        if (iter%128 == 0) { //update data structures
            vector<int> new_ids(0);
            int num_rem_points = 0;
            for (int c_id : remaining_center_ids) {
                if (quotas[c_id] > 0) {
                    new_ids.push_back(c_id);
                    num_rem_points += quotas[c_id];
                }
            }
            remaining_center_ids = move(new_ids);
            if (num_rem_points == 0) break;

            int rem_centers = remaining_center_ids.size();
            int rem_links = rem_centers * num_rem_points;
            if (rem_links < earlyCutoff) {
                bruteForceMatching(plane, centers, quotas,
                                    num_rem_points, rem_centers);
                break;
            }
        }
        iter++;
    }

    if (autoset_early_cutoff) setEarlyCutoff(-1);

    return plane;
}


void FastMatcher::bruteForceMatching(vector<vector<int>>& plane,
        const vector<Point>& centers, vector<int>& quotas,
        int num_rem_points, int num_rem_centers) {

    vector<Link> L = sortedRemLinks(plane, centers, quotas, num_rem_points, num_rem_centers);
    for (const Link& link : L) {
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

vector<Link> FastMatcher::sortedRemLinks(const vector<vector<int>>& plane,
        const std::vector<Point>& centers, const vector<int>& quotas,
        int num_rem_points, int num_rem_centers) {

    vector<int> rem_points(2*num_rem_points);

    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (plane[i][j] == -1) {
                rem_points[index] = i;
                rem_points[index+1] = j;
                index += 2;
            }
        }
    }
    if (index != (int)rem_points.size()) cout << "error: missing points" << endl;

    int k = quotas.size();
    vector<Link> L(num_rem_points*num_rem_centers);
    index = 0;
    for (int c_id = 0; c_id < k; c_id++) {
        if (quotas[c_id] > 0) {
            int ci = centers[c_id].i;
            int cj = centers[c_id].j;
            for (int p = 0; p < (int)rem_points.size(); p += 2) {
                int i = rem_points[p], j = rem_points[p+1];
                float dis_squared = (i-ci)*(i-ci)+(j-cj)*(j-cj);
                L[index] = Link(i, j, c_id, dis_squared);
                index++;
            }
        }
    }
    if (index != (int)L.size()) cout << "error: missing links" << endl;

    sort(L.begin(), L.end());
    return L;
}


