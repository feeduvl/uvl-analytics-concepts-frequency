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

using namespace std;


/**
 * Command line interface for frequency-based text analysis (model training or execution)
 */
int main(int argc, char** argv) {
    string command = argv[1];
    if(command == "test"){
        cout << R"({"topics":"Test successful."})" << endl;
    }
    else if(command == "train"){
        cout << "Training frequencies file." << endl;
        string concept_length = argv[2];
        string train_dir_path = argv[3];
        string outfile_path = argv[4];
        // build our frequency_model here using the algorithm of the next argument with the files in the directory given by the following argument
        auto faccepter = frequency_accepter(frequency_accepter::Mode::TRAIN_CORPUS);
        frequency_model m = frequency_model(stoi(concept_length));
        directory_walker walker = directory_walker<frequency_model>(train_dir_path, faccepter, m, false);
        m.write_concepts_to_file(outfile_path);
        //.write_omitted_words_to_file("stopwords.txt");

    } else if (command == "run_algorithm"){
        string concept_length_str = argv[2];
        int concept_length = stoi(concept_length_str);
        string algo_name = argv[3];

        string corpus_frequencies_file = argv[4];
        string stopwords_file = argv[5];
        string lemmatization_file = argv[6];

        string analyze_text = argv[7];
        int return_num_concepts = stoi(argv[8]);

        string name = argv[9];

        frequency_model model = frequency_model(concept_length);
        frequency_manager manager = frequency_manager(model);

        manager.read_corpus_frequencies_file(corpus_frequencies_file);
        manager.read_lemmatization_map(lemmatization_file);
        manager.read_stopwords(stopwords_file);

        if(algo_name=="rbai"){

            /*
             * Do: Relevance Based Abstraction Identification
             * Gacitua, R., Sawyer, P. & Gervasi, V. Relevance-based abstraction identification: technique and evaluation. Requirements Eng 16, 251 (2011). https://doi.org/10.1007/s00766-011-0122-3
             */

            json::JSON algo_return = manager.run_rbai(analyze_text, return_num_concepts, false);
            algo_return["name"] = name;
            // program output, then exit
            cout << algo_return << endl;

        } else if(algo_name == "fcic"){
            /*
             * Do: Finding Comparatively Important Concepts between texts
             * R. Lecoeuche, "Finding comparatively important concepts between texts," Proceedings ASE 2000. Fifteenth IEEE International Conference on Automated Software Engineering, 2000, pp. 55-60, doi: 10.1109/ASE.2000.873650.
             */

            string training_dir_path = argv[10];
            vector<string> candidate_concepts = manager.run_fcic(analyze_text, return_num_concepts, false);

            //cout << "Candidate concepts: " << str_util::vector_to_string(candidate_concepts) << endl;

            auto accepter_fcic = frequency_accepter(frequency_accepter::Mode::DEC_TREE);
            frequency_model m = frequency_model((unsigned int) concept_length);
            m.setCandidateTokensDecTree(candidate_concepts);
            directory_walker walker = directory_walker<frequency_model>(training_dir_path, accepter_fcic, m, true);

            vector<vector<bool>> decision_tree_data = manager.get_model().getDecTreeTrainingData();  // the data gathered from the input

            //cout << "Finished reading files. Sentences in input:  " <<  decision_tree_data.size() << endl;
            //cout << "Sentences in corpus: " << m.getDecTreeTrainingData().size() << endl;

            decision_tree_data.insert(decision_tree_data.end(), m.getDecTreeTrainingData().begin(), m.getDecTreeTrainingData().end());  // concatenate the two datasets

            DecisionTree dt;
            dt.attribute_names = candidate_concepts;
            vector<Example> examples;
            for(auto & line : decision_tree_data){
                bool target_value = line[line.size() - 1];
                line.pop_back();
                examples.emplace_back(line, target_value);
            }

            //cout << "Building decision tree... " << endl;

            dt.build(examples);
            //dt.print();
            vector<string> tree = dt.tree_to_vec();
            /*int index = 0;
            for(auto & s : tree){
                std::cout << s << "(" << index++<<")" << ",";
            }
            std::cout << str_util::vector_to_string(tree) << std::endl;*/

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

            cout << j << endl;

        } else {
            cerr << "Got undefined algorithm: '" << algo_name << "', exiting." << endl;
            return 1;
        }

    } else if(command == "find_occurences"){
        string text = argv[2];
        vector<string> find_lemmas;
        for(int i = 0; i < argc - 3; i++){
            find_lemmas.emplace_back(argv[i]);
        }

        frequency_model model = frequency_model(1); // arbitrary concept length
        frequency_manager manager = frequency_manager(model);  //  TODO expose this as a static method

        vector<unsigned int > occurences = manager.find_occurences(text, find_lemmas);

        json::JSON j;
        j["occurences"] = json::Array();

        for(int i = 0; i < occurences.size(); i++){
            j["occurences"][i] = occurences[i];
        }

        cout << j << endl;

    } else {
        cerr << "Got undefined command: '" << command << "', exiting." << endl;
        return 1;
    }


    return 0;
}



