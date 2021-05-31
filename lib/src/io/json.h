//
// Created by Jakob Weichselbaumer on 24.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_JSON_H
#define FEED_UVL_FINDING_COMPARATIVELY_JSON_H
#include <string>
#include <sstream>
#include <vector>
/**
 * Utility class to help build valid json strings
 */
class json {
    bool empty = true;
    std::stringstream ss;
public:
    json(){
        ss << '{' << ' ' << '\n';
    }

    template <class T> void add_attr(const string& name, T obj){
        if(!empty){
            ss << ',';
        }

        ss << '\"' << name << '\"' << ':' << '\"' << obj << '\"' << '\n';
        empty = false;
    }


    void add_list(const string& name, vector<double > & list){
        if(!empty){
            ss << ',';
        }
        ss << '\"' << name << '\"' << ':' << '[' << '\n';
        if(!list.empty()){
            for(int i = 0; i < list.size() -1; i++){
                ss << list[i] << ',' << '\n';
            }
            ss <<  list[list.size()-1]   << '\n';
        }
        ss << ']' << '\n';
        empty = false;
    }
    void add_list(const string& name, vector<string> & list){
        if(!empty){
            ss << ',';
        }
        ss << '\"' << name << '\"' << ':' << '[' << '\n';
        if(!list.empty()){
            for(int i = 0; i < list.size() -1; i++){
                ss << '\"' << list[i] << '\"' << ',' << '\n';
            }
            ss << '\"'  << list[list.size()-1] << '\"'  << '\n';
        }
        ss << ']' << '\n';
        empty = false;
    }

    string get_string(){
        ss << ' ' << '}';
        return ss.str();
    }
};
#endif //FEED_UVL_FINDING_COMPARATIVELY_JSON_H
