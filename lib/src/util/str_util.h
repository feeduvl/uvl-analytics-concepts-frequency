//
// Created by Jakob Weichselbaumer on 24.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H
#define FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <fstream>
#include <cmath>
#include <numeric>
#include <chrono>
#include <algorithm>
#include <string>

using namespace std;
class str_util{

public:
    static bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    static string vector_to_string(vector<int> & stringify){
        string next_parent_string;
        for(int & t : stringify){
            next_parent_string += (std::to_string(t) + " ");
        }
        if(next_parent_string!=""){
            next_parent_string.pop_back();
        }
        return next_parent_string;
    }
    static string vector_to_string(vector<string> & stringify){
        string next_parent_string;
        for(string & t : stringify){
            next_parent_string += (t + " ");
        }
        if(next_parent_string!=""){
            next_parent_string.pop_back();
        }
        return next_parent_string;
    }
};

#endif //FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H
