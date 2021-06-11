//
// Created by Jakob Weichselbaumer on 24.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H
#define FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H

#include <iostream>
#include <string>
#include "../../includes/rapidxml/rapidxml.hpp"
#include "../model/file_accepter.h"
#include "../model_builder/xml_reader.h"
#include "../model/frequency_model.h"
#include "../model/model_depth_wrapper.h"

class str_util{

public:
    static bool hasEnding (std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }
};

#endif //FEED_UVL_FINDING_COMPARATIVELY_STR_UTIL_H
