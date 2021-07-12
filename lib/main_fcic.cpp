#include <iostream>
#include <string>
#include <vector>

#include "src/io/json.h"
#include "src/model/model.h"
#include "src/model/frequency_accepter.h"
#include "src/model_builder/directory_walker.h"
#include "src/model_builder/frequency_manager.h"
#include "src/dtree/decision_tree.h"
#include "src/util/str_util.h"
#include "./find_occurences.h"

using namespace std;

/*
 * Do: Finding Comparatively Important Concepts between texts
 * R. Lecoeuche, "Finding comparatively important concepts between texts," Proceedings ASE 2000. Fifteenth IEEE International Conference on Automated Software Engineering, 2000, pp. 55-60, doi: 10.1109/ASE.2000.873650.
 */

int main(int argc, char** argv) {
    string command = argv[1];
    if (command == "run_algorithm"){
        string concept_length_str = argv[2];
        int concept_length = stoi(concept_length_str);

        string corpus_frequencies_file = argv[3];
        string stopwords_file = argv[4];
        string lemmatization_file = argv[5];

        string analyze_text = argv[6];
        int return_num_concepts = stoi(argv[7]);

        string name = argv[8];

        frequency_model model = frequency_model(concept_length);
        frequency_manager manager = frequency_manager(model);

        /*
         * Read in word frequency statistics - use these to find candidate concepts
         */
        manager.read_corpus_frequencies_file(corpus_frequencies_file);
        manager.read_lemmatization_map(lemmatization_file);
        manager.read_stopwords(stopwords_file);

        string training_dir_path = argv[10];
        vector<string> candidate_concepts = manager.run_fcic(analyze_text, return_num_concepts, false);

        /*
         * Gather data for decision tree
         */
        auto accepter_fcic = frequency_accepter(frequency_accepter::Mode::DEC_TREE);
        frequency_model m = frequency_model((unsigned int) concept_length);
        m.setCandidateTokensDecTree(candidate_concepts);
        directory_walker walker = directory_walker<frequency_model>(training_dir_path, accepter_fcic, m, true);

        vector<vector<bool>> decision_tree_data = manager.get_model().getDecTreeTrainingData();  // the data gathered from the input

        decision_tree_data.insert(decision_tree_data.end(), m.getDecTreeTrainingData().begin(), m.getDecTreeTrainingData().end());  // concatenate the two datasets

        DecisionTree dt;
        dt.attribute_names = candidate_concepts;
        vector<Example> examples;
        for(auto & line : decision_tree_data){
            bool target_value = line[line.size() - 1];
            line.pop_back();
            examples.emplace_back(line, target_value);
        }

        /*
         * Make the decision tree (split by information gain)
         */
        dt.build(examples);
        vector<string> tree = dt.tree_to_vec();

        json::JSON j;

        j["name"] = name;

        j["topics"] = json::Object();
        j["topics"]["concepts"] = json::Array();
        j["topics"]["information_gain"] = json::Array();
        j["topics"]["tree"] = json::Array();

        for(int i = 0; i < candidate_concepts.size(); i++){
            j["topics"]["concepts"][i] = candidate_concepts[i];
            j["topics"]["information_gain"][i] = dt.information_gain[i];
        }
        for(int i = 0; i < tree.size(); i++){
            j["topics"]["tree"][i] = tree[i];
        }

        // Program output
        cout << j << endl;

    } else if(command == "find_occurences"){
        string text = argv[2];
        unsigned int concept_length = stoi(argv[3]);
        vector<string> find_lemmas;
        find_lemmas.reserve(argc - 4);
        for(int i = 0; i < argc - 4; i++){
            find_lemmas.emplace_back(argv[4+i]);
        }

        json::JSON j = find_occurences(text, find_lemmas, concept_length);

        cout << j << endl;

    } else {
        cerr << "Got undefined command: '" << command << "', exiting." << endl;
        return 1;
    }


    return 0;
}



