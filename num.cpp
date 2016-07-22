#include "num.h"

const double Num::DINF = DBL_MAX;
const string Num::SINF = "inf";

Num::Num(): val(0) {}

Num::Num(double val): val(val) {}

Num::Num(const QString &qstr): val(qstr2NumVal(qstr)) {   }

Num::Num(const string &str): val(qstr2NumVal(QString::fromStdString(str))) {}

Num::Num(const Num &num): val(num.val) {}

Num Num::inf() {
    return Num(DINF);
}

bool Num::isInf() const {
    return val == DINF;
}

bool Num::isInt() const {
    return val != DINF && floor(val) == val;
}

int Num::toInt() const {
    if (!isInt()) throw runtime_error("is not int");
    return (int)floor(val);
}

double Num::toDouble() const {
    if (val == DINF) throw runtime_error("Is infinity");
    return val;
}

string Num::toStr() const {
    if (val == DINF) return SINF;
    if (isInt()) return to_string(toInt());
    return to_string(val);
}

QString Num::toQstr() const {
    return QString::fromStdString(toStr());
}

bool Num::isNum(const QString &str) {
    if (str == SINF.c_str()) return true;
    bool ok;
    str.toDouble(&ok);
    return ok;
}

bool Num::operator==(const Num &rhs) const {
    return val == rhs.val;
}

double Num::qstr2NumVal(const QString &qstr) {
    if (qstr == SINF.c_str()) return DINF;
    bool ok;
    double val = qstr.toDouble(&ok);
    if (!ok) throw runtime_error("Invalid string");
    return val;
}
