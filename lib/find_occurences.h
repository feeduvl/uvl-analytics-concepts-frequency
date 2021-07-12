//
// Created by Jakob Weichselbaumer on 12.07.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_FIND_OCCURENCES_H
#define FEED_UVL_FINDING_COMPARATIVELY_FIND_OCCURENCES_H

#include <vector>
#include <string>
#include "src/io/json.h"
#include "src/model/model.h"
#include "src/model/frequency_accepter.h"
#include "src/model_builder/directory_walker.h"
#include "src/model_builder/frequency_manager.h"

using namespace std;
json::JSON find_occurences(string & text, vector<string> & concepts, unsigned int & concept_length){

    frequency_model model = frequency_model(1); // arbitrary concept length
    frequency_manager manager = frequency_manager(model);  //  TODO expose this as a static method

    vector<unsigned int > occurences = manager.find_occurences(text, concept_length,concepts);

    json::JSON j;
    j["occurences"] = json::Array();

    for(int i = 0; i < occurences.size(); i++){
        j["occurences"][i] = occurences[i];
    }

    return j;
}

#endif //FEED_UVL_FINDING_COMPARATIVELY_FIND_OCCURENCES_H
