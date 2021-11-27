#include <iostream>
#include <vector>
#include <fstream>
#include "sat.h"

using namespace std;

vector<vector<int> > neighbor(size_t i, size_t j){
    int tmp[5] = {-1, 0, 1, 0, -1};
    vector<vector<int> > ans;
    int k, l;
    for(int t = 0; t < 4; t++){
        k = i + tmp[t];
        l = j + tmp[t + 1];
        if(k < 0 || k > 3 || l < 0 || l > 3) continue;
        vector<int> a;
        a.push_back(k); a.push_back(l);
        ans.push_back(a);
    }
    return ans;
}
