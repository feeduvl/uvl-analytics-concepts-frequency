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
#include "../../includes/rapidxml/rapidxml.hpp"
#include "../model/file_accepter.h"
#include "../model_builder/xml_reader.h"
#include "../model/frequency_model.h"
#include "../model/model_depth_wrapper.h"
#include "../graph/concept_node.h"
#include "../util/str_util.h"
#include "../io/json.h"

class str_util{

public:
    static bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }

    static string vector_to_string(vector<string> & next_parent){
        string next_parent_string;
        for(string & t : next_parent){
            next_parent_string += (t + " ");
        }
        next_parent_string.pop_back();
        return next_parent_string;
    }
};

#endif //FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H
