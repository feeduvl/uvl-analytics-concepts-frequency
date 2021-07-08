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

    template <class T> static string vector_to_string(vector<T> & stringify){
        string next_parent_string;
        for(T & t : stringify){
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

    static size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
    {
        size_t pos = txt.find( ch );
        size_t initialPos = 0;
        strs.clear();

        // Decompose statement
        while( pos != std::string::npos ) {
            strs.push_back( txt.substr( initialPos, pos - initialPos ) );
            initialPos = pos + 1;

            pos = txt.find( ch, initialPos );
        }

        // Add the last one
        strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

        return strs.size();
    }
};

#endif //FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H
