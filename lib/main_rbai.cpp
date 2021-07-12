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
 * Implementation of: Relevance Based Abstraction Identification
 * Gacitua, R., Sawyer, P. & Gervasi, V. Relevance-based abstraction identification: technique and evaluation. Requirements Eng 16, 251 (2011). https://doi.org/10.1007/s00766-011-0122-3
 */

int main(int argc, char** argv) {
    string command = argv[1];
    if(command == "train"){
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

        string corpus_frequencies_file = argv[3];
        string stopwords_file = argv[4];
        string lemmatization_file = argv[5];

        string analyze_text = argv[6];
        int return_num_concepts = stoi(argv[7]);

        string name = argv[8];

        frequency_model model = frequency_model(concept_length);
        frequency_manager manager = frequency_manager(model);

        manager.read_corpus_frequencies_file(corpus_frequencies_file);
        manager.read_lemmatization_map(lemmatization_file);
        manager.read_stopwords(stopwords_file);

        json::JSON algo_return = manager.run_rbai(analyze_text, return_num_concepts, false);
        algo_return["name"] = name;
        // program output, then exit
        cout << algo_return << endl;

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



