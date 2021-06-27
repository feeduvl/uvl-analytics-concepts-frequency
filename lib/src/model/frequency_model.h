//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MODEL_H
#define FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MODEL_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <vector>

#include "../graph/concept_node.h"
#include "../model/model.h"
#include "../graph/concept_node.h"

using namespace std;

class frequency_model : public model<string> {

    mutable vector<string> candidate_tokens_dec_tree = vector<string>();
    mutable vector<vector<bool>> dec_tree_training_data = vector<vector<bool>>();


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

    frequency_model(unsigned int t_l) : model(t_l){

    }

    [[nodiscard]] concept_node<string> * getRootNode() const {
        return root_node;
    }

    /**
     * Print out all corpus_concepts_sorted gathered in the frequency_model
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

    vector<bool> get_empty_sentence(vector<string> & candidate_concepts) const{
        vector<bool> empty_sentence = vector<bool>(candidate_concepts.size(), false);
        return empty_sentence;
    }

    vector<bool> get_empty_sentence() const{
        return get_empty_sentence(candidate_tokens_dec_tree);
    }


    void add_dec_tree_sentence(vector<bool> & sentence, bool label) const {
        dec_tree_training_data.push_back(sentence);
        dec_tree_training_data[dec_tree_training_data.size()-1].push_back(label);
    }

    void process_token_dec_tree(vector<bool> & sentence_data, const string & token) const{
        for(int i = 0; i < candidate_tokens_dec_tree.size(); i++){
            if(!sentence_data[i]){
                if(token == candidate_tokens_dec_tree[i]){
                    sentence_data[i] = true;
                    return;
                }
            }
        }
    }

    void setCandidateTokensDecTree(const vector<string> &candidateTokensDecTree) const {
        candidate_tokens_dec_tree = candidateTokensDecTree;
    }

    const vector<vector<bool>> &getDecTreeTrainingData() const {
        return dec_tree_training_data;
    }
};


#endif //FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MODEL_H
