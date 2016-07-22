#include "edouble.h"
#include <string>

using namespace std;

EDouble::EDouble()
{
    inf = false;
    val = 0;
}

string EDouble::str() const {
    if (inf) return "inf";
    else if (isEInt()) return getEInt().str();
    return to_string(val);
}

QString EDouble::qstr() const
{
    return QString::fromStdString(str());
}

double EDouble::toDouble() const
{
    if (inf) throw runtime_error("Is infinity");
    return val;
}

void EDouble::setVal(QString str) {
    if (str == "inf") {
        inf = true;
    } else {
        inf = false;
        bool ok;
        val = str.toDouble(&ok);
        if (!ok) {
            throw runtime_error("Invalid string");
        }
    }
}

bool EDouble::isEInt() const
{
    return inf || floor(val) == val;
}

EInt EDouble::getEInt() const
{
    return EInt(inf, (int)floor(val));
}

EDouble::EDouble(QString str)
{
    setVal(str);
}

EDouble::EDouble(string str)
{
    setVal(QString::fromStdString(str));
}



bool EDouble::isEDouble(QString str)
{
    if (str == "inf") return true;
    bool ok;
    str.toDouble(&ok);
    return ok;
}
