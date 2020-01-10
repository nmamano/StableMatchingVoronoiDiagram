#ifndef PRESORT_H
#define PRESORT_H
#include "cpstruct.h"
#include "matchingutils.h"
#include <unordered_map>

struct PresortedArray {
    PresortedArray(const vector<int>& array):
        array(array), ind(0) {}
    int curr() {
        return array[ind];
    }
    void skip() {
        ind++;
    }

private:
    const vector<int> array;
    int ind;


};

class Presort : public CPStruct
{
public:

    Presort(Metric metric, const vector<Point> &points,
            const vector<int>& inds, const vector<Point>& queries):
        CPStruct(metric, points), remPoints(false) {

        for (int ind : inds) {
            remPoints[ind] = true;
        }

        for (const Point& q : queries) {
            vector<int> centers = centersSortedByDist(q, inds);
            PresortedArray array(centers);
            arrays[q] = array;
        }

    }

    string acronym() const {return "PR";}

    void delInd(int ind) {
        remPoints[ind] = false;
    }

    int closestInd(const Point &p) const {
        PresortedArray array = arrays[p];
        while (!remPoints[array.curr()]) array.skip();
        return array.curr();
    }

private:
    vector<bool> remPoints;
    unordered_map<Point,PresortedArray> arrays;


    vector<int> centersSortedByDist(Point site, const vector<int>& cIds) const {
        int k = cIds.size();
        vector<int> res(k);
        if (!real) {
            vector<IntDistIdPair> idDists = centersSortedByIntDist(site, cIds);
            for (int i = 0; i < k; i++) res[i] = idDists[i].id;
        } else {
            vector<RealDistIdPair> idDists = centersSortedByRealDist(site, cIds);
            for (int i = 0; i < k; i++) res[i] = idDists[i].id;
        }
        return res;
    }

    vector<IntDistIdPair> centersSortedByIntDist(Point site, const vector<int>& cIds) const {

        vector<IntDistIdPair> pairs(0);
        pairs.reserve(cIds.size());
        for (int cId : cIds) {
            IntDistIdPair p(intDist(site, cId), cId);
            pairs.push_back(p);
        }
        radix_sort(pairs);
        return pairs;
    }

    vector<RealDistIdPair> centersSortedByRealDist(Point site, const vector<int>& cIds) const {

        vector<RealDistIdPair> pairs(0);
        pairs.reserve(cIds.size());
        for (int cId : cIds) {
            RealDistIdPair p(realDist(site, cId), cId);
            pairs.push_back(p);
        }
        sort(pairs.begin(), pairs.end());
        return pairs;
    }
};

#endif // PRESORT_H
