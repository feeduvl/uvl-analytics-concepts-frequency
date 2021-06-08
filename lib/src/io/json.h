//
// Created by Jakob Weichselbaumer on 24.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_JSON_H
#define FEED_UVL_FINDING_COMPARATIVELY_JSON_H
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

/**
 * Utility class to help build valid json strings
 */
class json {
    bool empty;
    std::stringstream ss;

    std::string escape_json(const std::string &s) {
        std::ostringstream o;
        for (auto c = s.cbegin(); c != s.cend(); c++) {
            switch (*c) {
                case '"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b"; break;
                case '\f': o << "\\f"; break;
                case '\n': o << "\\n"; break;
                case '\r': o << "\\r"; break;
                case '\t': o << "\\t"; break;
                default:
                    if ('\x00' <= *c && *c <= '\x1f') {
                        o << "\\u"
                          << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
                    } else {
                        o << *c;
                    }
            }
        }
        return o.str();
    }

public:
    json(){
        ss << '{' << ' ' << '\n';
        empty = true;
    }

    template <class T> void add_attr(const std::string& name, T obj, bool quotes){
        using namespace std;
        if(!empty){
            ss << ',';
        }

        std::ostringstream oss {};
        oss << obj;
        std::string s = oss.str();

        ss << '\"' << name << '\"' <<  ':' << (quotes?"\"":"") << escape_json(s) << (quotes?"\"":"") << '\n';
        empty = false;
    }


    void add_list(const std::string& name, std::vector<double > & list){
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
    void add_list(const std::string& name, std::vector<std::string> & list){
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

    std::string get_string(){
        ss << ' ' << '}';
        return ss.str();
    }
};
#endif //FEED_UVL_FINDING_COMPARATIVELY_JSON_H
