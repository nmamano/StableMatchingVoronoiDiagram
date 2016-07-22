#ifndef EDOUBLE_H
#define EDOUBLE_H

#include <string>
#include <QString>
#include "eint.h"
using namespace std;

class EDouble
{
public:
    bool inf;
    double val;

    EDouble();
    EDouble(QString str);
    EDouble(string str);
    EDouble(double val): inf(false), val(val) {}
    EDouble(EInt eint): inf(eint.inf), val(eint.val) {}
    EDouble(int val): inf(false), val(val) {}
    EDouble(bool inf, int val): inf(inf), val(val) {}

    string str() const;
    QString qstr() const;
    double toDouble() const;

    void setVal(QString str);
    static bool isEDouble(QString str);

    bool isEInt() const;
    EInt getEInt() const;

    bool operator==(const EDouble& rhs) const {
        return (inf && rhs.inf) || (!inf && !rhs.inf && val == rhs.val);
    }
    friend ostream& operator<<(ostream& out, const EDouble& edouble) {
        out << edouble.str();
        return out;
    }

};

#endif // EDOUBLE_H
