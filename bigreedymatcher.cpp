#include "bigreedymatcher.h"

void BiGreedyMatcher::setMetric(const Metric &metric) {
    this->metric = metric;
}

vector<int> BiGreedyMatcher::remainingCenterIds(const vector<int> &quotas) {
    int k = quotas.size();
    vector<int> res(0);
    for (int i = 0; i < k; i++) {
        if (quotas[i] > 0)
            res.push_back(i);
    }
    return res;
}

vector<Point> BiGreedyMatcher::remainingPoints(const vector<vector<int> > &plane, int numRemPoints) {
    int count = 0;
    int n = plane.size();
    vector<Point> res(numRemPoints);
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
