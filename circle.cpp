#include "circle.h"
#include "metric.h"

Circle::Circle(NPoint center, double radius) : center(center), radius(radius)
{

}

bool Circle::contains(NPoint q) const
{
    Metric L2(2);
    return L2.ddist(q, center) <= radius;
}
