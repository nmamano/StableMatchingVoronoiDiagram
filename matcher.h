#ifndef MATCHER_H
#define MATCHER_H

#include <vector>
#include "matchingutils.h"

using namespace std;

class Matcher
{
public:
    Matcher(int n, int distMetric = 2);
    vector<vector<int>> query(const vector<Point>& centers) const;
    int tailoredCutoff(int k) const;

    void setN(int newN);
    void setMetric(int newMetric);
    void setCutoff(int cutoff) { earlyCutoff = cutoff; }
    int getMetric() {return distMetric;}
    void step(vector<vector<int>> &plane, const vector<Point> &centers, int iter);
private:
    int n;
    int distMetric;
    int earlyCutoff;
    vector<Point> P;

    void buildP();

    void innerLoop(int pi, int pj,
            const vector<int>& c_ids, const vector<Point>& centers,
            vector<vector<int>>& plane, vector<int>& quotas) const;

    void middleLoop(int i, int j,
            const vector<int> &c_ids, const vector<Point> &centers,
            vector<vector<int> > &plane, vector<int> &quotas) const;


    void bruteForceMatching(vector<vector<int>> &plane, const vector<Point>& centers, vector<int> &quotas, int num_rem_points, int num_rem_centers) const;
    vector<Link> sortedRemLinks(const vector<vector<int>> &plane, const vector<Point>& centers, const vector<int> &quotas, int num_rem_points, int num_rem_centers) const;

};

#endif // MATCHER_H
