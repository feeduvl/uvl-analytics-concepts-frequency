//
// Created by Jakob Weichselbaumer on 15.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_MODEL_DEPTH_WRAPPER_H
#define FEED_UVL_FINDING_COMPARATIVELY_MODEL_DEPTH_WRAPPER_H

#include <vector>
#include <string>
#include "../graph/concept_node.h"

/**
 * Provide access to the constituent words of a term within the model's data structure
 */
template <class model_type> struct model_depth_wrapper {

    const model_type & m;
    mutable vector<concept_node<string>*> list = vector<concept_node<string>*>();  //  concept length of n <=> list of n stored concept nodes
    mutable unsigned int current_full_concept = 0; // index of the current/next concept to reach full length
    mutable unsigned int current_word_index = 0;  //  only need to count to the term length
    mutable bool pipeline_loaded = false;

    explicit model_depth_wrapper(const model_type & model) : m(model){
        reset_sentence();
    }

    void next_word(){
        if(!pipeline_loaded){
            current_word_index++;
        }
        if(current_word_index == m.getTermLength()){
            pipeline_loaded = true;
            if(current_full_concept == m.getTermLength()){
                current_full_concept = 0;
            }
            list[current_full_concept] = m.getRootNode();
            ++current_full_concept;
        }
    }

    void reset_sentence(){
        pipeline_loaded = false;
        current_word_index = 0;
        current_full_concept = 0;
        list = vector<concept_node<string>*>(m.getTermLength(), m.getRootNode());
    }

};

#endif //FEED_UVL_FINDING_COMPARATIVELY_MODEL_DEPTH_WRAPPER_H
