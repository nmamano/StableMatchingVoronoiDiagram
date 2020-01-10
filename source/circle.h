#ifndef CIRCLE_H
#define CIRCLE_H
#include "npoint.h"

class Circle
{
public:
    double radius;
    NPoint center;
    Circle() {}
    Circle(NPoint center, double radius);
    bool contains(NPoint q) const;
};

#endif // CIRCLE_H
