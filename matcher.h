#ifndef MATCHER_H
#define MATCHER_H

#include <vector>
#include "matchingutils.h"

using namespace std;

class Matcher
{
public:

    static vector<vector<int>> assignPoints(int n,
            const vector<Point>& centers);

private:

    static vector<Link> sortedLinks(int n, const vector<Point>& centers);


};

#endif // MATCHER_H
