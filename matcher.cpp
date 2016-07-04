#include "matcher.h"
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;

Matcher::Matcher(int n, int distMetric):
    n(n), distMetric(distMetric)
{
    earlyCutoff = -1;
    buildP();
}

void Matcher::setN(int newN) {
    if (newN == n) return;
    n = newN;
    earlyCutoff = -1;
    if ((int)P.size() < n*(n+1)/2) buildP();
}

void Matcher::setMetric(int newMetric) {
    if (newMetric == distMetric) return;
    distMetric = newMetric;
    buildP();
}

void Matcher::buildP() {
    P = vector<Point>(n*(n+1)/2);
    int index = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j <= i; j++) {
            P[index] = Point(i,j);
            index++;
        }
    }
    if (index != (int)P.size()) cout<<"Error: missing points"<<endl;

    if (distMetric == 1) sort(P.begin(), P.end(), Point::sortL1);
    else if (distMetric == 2) sort(P.begin(), P.end(), Point::sortL2);
    else if (distMetric == -1) sort(P.begin(), P.end(), Point::sortLInfty);
    else cout << "Error: unknown distance metric" << endl;

}

int Matcher::tailoredCutoff(int k) const {
    return 500*cbrt(n*n*k);
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

vector<vector<int>> Matcher::query(const vector<Point> &centers) const
{
    int k = centers.size();

    int cutoff = earlyCutoff == -1 ? tailoredCutoff(k) : earlyCutoff;

    vector<vector<int>> plane(n, vector<int> (n, -1));
    vector<int> quotas = centerQuotas(n, k);

    vector<int> rem_c_ids(k);
    for (int i = 0; i < k; i++) rem_c_ids[i] = i;

    int iter = 0;
    for (const Point& p : P) {
        middleLoop(p.i, p.j, rem_c_ids, centers, plane, quotas);

        if (iter%64 == 0) { //update data structs from time to time
            vector<int> new_ids(0);
            int num_rem_points = 0;
            for (int c_id : rem_c_ids) {
                if (quotas[c_id] > 0) {
                    new_ids.push_back(c_id);
                    num_rem_points += quotas[c_id];
                }
            }
            rem_c_ids = move(new_ids);
            if (num_rem_points == 0) break;

            int rem_centers = rem_c_ids.size();
            int rem_links = rem_centers * num_rem_points;
            if (rem_links < cutoff) {
                bruteForceMatching(plane, centers, quotas,
                        num_rem_points, rem_centers);
                break;
            }
        }
        iter++;
    }

    return plane;
}

void Matcher::bruteForceMatching(vector<vector<int>>& plane,
        const vector<Point>& centers, vector<int>& quotas,
        int num_rem_points, int num_rem_centers) const {

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

vector<Link> Matcher::sortedRemLinks(const vector<vector<int>>& plane,
        const std::vector<Point>& centers, const vector<int>& quotas,
        int num_rem_points, int num_rem_centers) const {

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
                double dis = Point::dist(i, j, ci, cj, distMetric);
                L[index] = Link(i, j, c_id, dis);
                index++;
            }
        }
    }
    if (index != (int)L.size()) cout << "error: missing links" << endl;

    sort(L.begin(), L.end());
    return L;
}


