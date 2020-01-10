#include "linearsearch.h"


void LinearSearch::delInd(int ind) {
//    cerr<<"del point "<<ind<<endl;
    inds.erase(ind);
}

int LinearSearch::closestInd(const Point &p) const {
    int minInd = inds.elems[0];
    double minDis = dist(p, minInd);

    for (int i = 1; i < (int)inds.elems.size(); i++) {
        int ind = inds.elems[i];
        double dis = dist(p, ind);
        if (dis < minDis) {
            minDis = dis;
            minInd = ind;
        }
    }
//    cerr<<"closest ind "<<minInd<<" "<<minDis<<endl;
//    cerr<<"elems: ";
//    for(int x:inds.elems)cerr<<x<<" ";cout<<endl;
    return minInd;
}
