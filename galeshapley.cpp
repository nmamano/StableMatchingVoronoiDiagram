#include "galeshapley.h"
#include <unordered_map>
#include <iostream>

GaleShapley::GaleShapley()
{

}

struct Student {
    int id;
    int hospitalId;
    vector<double> dists;
    Student() {}
    Student(int id, const vector<double>& dists) {
        this->id = id;
        hospitalId = -1;
        this->dists = dists;
    }

    bool shouldAccept(int HId) {
        if (hospitalId == -1) {
            return true;
        }
        return dists[HId] < dists[hospitalId];
    }
};

struct Hospital {
    int id;
    int quota;
    vector<int> prefs;
    int prefInd;

    Hospital() {}
    Hospital(int id, int quota, const vector<int>& prefs) {
        this->id = id;
        this->quota = quota;
        this->prefs = prefs;
        prefInd = 0;
    }

    int next() {
        if (prefInd >= (int)prefs.size()) cerr<<"error"<<endl;
        int res = prefs[prefInd];
        prefInd = prefInd + 1;
        return res;
    }

    bool done() {
        return prefInd == (int)prefs.size();
    }
};

/*
the id of a student is its index in studentDists
the id of a hospital is its index in hospitalPrefs

studentDists has, for each student, a value for each
hospital that can be compared to decide which one it prefers
it can be the rank, but not necessarily

hospitalPrefs has, for each hospital, the student ids
sorted by preference

quotas has, for each hospital,
how many students it can match at most

returns a vector mapping students to hsopital ids
  */

vector<int> GaleShapley::solve(const vector<vector<double> > &studentDists,
        const vector<vector<int> > &hospitalPrefs, const vector<int> &quotas)
{
    int n = studentDists.size();
    int k = hospitalPrefs.size();
    vector<Student> students(n);
    vector<Hospital> hospitals(k);
    vector<int> unmatched(k);
    for (int i = 0; i < n; i++) {
        students[i] = Student(i, studentDists[i]);
    }
    for (int i = 0; i < k; i++) {
        hospitals[i] = Hospital(i, quotas[i], hospitalPrefs[i]);
        unmatched[i] = i;
    }

    while (not unmatched.empty()) {
        int HId = unmatched[unmatched.size()-1];
        Hospital& H = hospitals[HId];
        int SId = H.next();
        Student& S = students[SId];
        if (S.shouldAccept(HId)) {
            int oldHospId = S.hospitalId;
            S.hospitalId = HId;
            H.quota -= 1;
            if (H.quota == 0 || H.done()) unmatched.pop_back();

            if (oldHospId != -1) {
                Hospital& oldH = hospitals[oldHospId];
                oldH.quota += 1;
                if (oldH.quota == 1 && not oldH.done()) {
                    unmatched.push_back(oldHospId);
                }
            }
        } else if (H.done()) {
            unmatched.pop_back();
        }
    }
    vector<int> res(n);
    for (int i = 0; i < n; i++) {
        res[i] = students[i].hospitalId;
    }

//    for(int i =0;i<n;i++)cerr<<res[i]<<" ";cerr<<endl;
//    for (int i=0;i<n;i++){
//        cerr<<"face "<<i<<": ";
//        for (int j = 0; j < k; j++)cerr<<studentDists[i][j]<<" ";
//        cerr<<endl;
//    }
//    for (int j=0;j<k;j++) {
//        cerr<<"center "<<j<<": ";
//        for(int i=0;i<n;i++)cerr<<hospitalPrefs[j][i]<<" ";
//        cerr<<endl;
//    }

    for (int i = 0; i < n; i++) {//for each student i
        for (int j = 0; j < k; j++) {//for each hospital j
            if (studentDists[i][j] < studentDists[i][res[i]]) {//preferred by i to its match
                for (int ii = 0; ii < n; ii++) {//for each student ii
                    if (res[ii] == j) {//matched to j
                        //j must prefer ii to i
                        for (int k = 0; k < n; k++) {
                            if (hospitalPrefs[j][k] == ii) break;
                            if (hospitalPrefs[j][k] == i) cout << "unstable" << endl;
                        }
                    }
                }
            }
        }
    }
    return res;
}

