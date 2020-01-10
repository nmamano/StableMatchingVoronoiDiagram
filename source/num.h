#ifndef NUM_H
#define NUM_H

#include <float.h>
#include <string>
#include <QString>
using namespace std;

class Num
{
public:
    static const double DINF; //larger than any other possible value
    static const string SINF; //"inf"

    double val;
    Num();
    Num(double val);
    Num(const QString& qstr);
    Num(const string& str);
    Num(const Num& num);
    static Num inf();

    bool isInf() const;
    bool isInt() const;
    static bool isNum(const QString& str);

    int asInt() const;
    double asDouble() const;
    string asStr() const;
    QString asQstr() const;

    int round() const;

    bool operator==(const Num& rhs) const;
    friend ostream& operator<<(ostream& out, const Num& num) {
        out << num.asStr();
        return out;
    }

private:
    static double qstr2NumVal(const QString& qstr);

};


#endif // NUM_H
