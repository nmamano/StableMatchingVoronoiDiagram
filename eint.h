#ifndef EINT_H
#define EINT_H

#include <string>
#include <QString>

using namespace std;

//Extended int. Can be a normal value or infinite

class EInt
{
public:
    bool inf;
    int val;

    EInt(): inf(false), val(0) {}
    EInt(bool inf, int val): inf(inf), val(val) {}
    EInt(int val): inf(false), val(val) {}
    EInt(QString str);
    EInt(string str);

    std::string str() const;
    QString qstr() const;
    int toInt() const;

    void setVal(QString str);
    static isEInt(QString str);

    bool operator==(const EInt& rhs) const {
        return (inf && rhs.inf) || (!inf && !rhs.inf && val == rhs.val);
    }
    friend ostream& operator<<(ostream& out, const EInt& eint) {
        out << eint.str();
        return out;
    }
};

#endif // EINT_H
