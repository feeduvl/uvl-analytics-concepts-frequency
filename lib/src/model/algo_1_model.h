//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_ALGO_1_MODEL_H
#define FEED_UVL_FINDING_COMPARATIVELY_ALGO_1_MODEL_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>

#include "src/graph/concept_node.h"
#include "src/model/model.h"
#include "../graph/concept_node.h"

using namespace std;

class algo_1_model : public model<string> {

    mutable vector<string> omitted_words = vector<string>();

    concept_node<string> * root_node = new concept_node<string>(nullptr, 0);
    static void recurse_print_concepts(concept_node<string> * node, const string& parent_concept){
        for(concept_node<string> * c : node->children){
            string next_parent = parent_concept + " " + c->getKey();
            cout << next_parent << endl;
            recurse_print_concepts(c, next_parent);
        }
    }

    static void recurse_write_to_file(concept_node<string> * node, ofstream & file, const string & parent_concept, bool first){
        for(concept_node<string> * c : node->children){
            string next_parent;
            if(first){
                next_parent = c->getKey();
            } else {
                next_parent = parent_concept + " " + c->getKey();
            }
            string writeLine = to_string(c->depth) + " " + next_parent + " " + to_string(c->concept_frequency);
            file << writeLine << "\n";
            recurse_write_to_file(c, file, next_parent, false);
        }
    }

public:
    [[nodiscard]] concept_node<string> * getRootNode() const {
        return root_node;
    }

    /**
     * Print out all corpus_concepts_sorted gathered in the algo_1_model
     */
    void recurse_print_concepts() const{
        recurse_print_concepts(root_node, "");
    }

    void write_concepts_to_file(const string & filename){
        ofstream FrequenciesFile(filename);
        FrequenciesFile << total_words_into_model << " " << l1_words << " " << concepts << "\n";
        recurse_write_to_file(root_node, FrequenciesFile, "", true);
        FrequenciesFile.close();
        cout << endl << endl << endl << endl;
        cout << "Wrote frequencies to file: " << filename << " in directory: " << filesystem::current_path() << endl;
    }

    void write_omitted_words_to_file(const string & omitted_words_filename){
        ofstream OmissionsFile(omitted_words_filename);
        for(auto & omitted_word : omitted_words){
            OmissionsFile << omitted_word << endl;
        }
        cout << "Wrote stop words / word omissions to file: " << omitted_words_filename << " in directory: " << filesystem::current_path() << endl;
    }

    void omit_word(const string& word) const{
        const auto it = lower_bound(omitted_words.begin(), omitted_words.end(), word);
        if(it == omitted_words.end()){
            omitted_words.push_back(word);
        } else {
            if(*it != word){
                omitted_words.insert(it, word);
            }
        }
    }
};


#endif //FEED_UVL_FINDING_COMPARATIVELY_ALGO_1_MODEL_H
