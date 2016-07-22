#include "eint.h"
#include <string>

using namespace std;



string EInt::str() const {
    if (inf) return "inf";
    return to_string(val);
}

QString EInt::qstr() const {
    return QString::fromStdString(str());
}

int EInt::toInt() const {
    if (inf) throw runtime_error("Is not int");
    return val;
}

void EInt::setVal(QString str) {
    if (str == "inf") {
        inf = true;
    } else {
        inf = false;
        bool ok;
        val = str.toInt(&ok);
        if (!ok) {
            throw runtime_error("Invalid string");
        }
    }
}

EInt::EInt(QString str)
{
    setVal(str);
}

EInt::EInt(string str) {
    setVal(QString::fromStdString(str));
}



EInt::isEInt(QString str)
{
    if (str == "inf") return true;
    bool ok;
    str.toInt(&ok);
    return ok;
}
