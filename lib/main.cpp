#include <iostream>
#include <string>
#include <vector>

#include "src/io/json.h"
#include "src/model/model.h"
#include "src/model/accepter_rbai.h"
#include "src/model_builder/directory_walker.h"
#include "src/model_builder/frequency_manager.h"
using namespace std;


/**
 * Command line interface for frequency-based text analysis (model training or execution)
 */
int main(int argc, char** argv) {
    string command = argv[1];
    //cout << "Command: "<< command << endl;
    //cout << "Concept length: " << concept_length << endl;

    if(command == "test"){
        cout << R"({"topics":"Test successful."})" << endl;
    }
    else if(command == "train_frequencies"){
        string concept_length = argv[2];
        // build our frequency_rbai_model here using the algorithm of the next argument with the files in the directory given by the following argument
        accepter_rbai accepterAlgo1 = accepter_rbai();
        frequency_rbai_model m = frequency_rbai_model();
        m.setTermLength(stoi(concept_length));
        directory_walker walker = directory_walker<frequency_rbai_model>(argv[4], accepterAlgo1, m);
        m.write_concepts_to_file("res/frequencies.txt");
        //.write_omitted_words_to_file("stopwords.txt");

    } else if (command == "run_algorithm"){
        string concept_length = argv[2];
        //  run the algorithm as defined by the following arguments
        string algo_name = argv[3];

        string corpus_frequencies_file = argv[4];
        string stopwords_file = argv[5];
        string lemmatization_file = argv[6];

        string analyze_text = argv[7];
        int return_num_concepts = stoi(argv[8]);

        frequency_rbai_model m = frequency_rbai_model();
        m.setTermLength((unsigned int) stoi(concept_length));
        frequency_manager<frequency_rbai_model> manager = frequency_manager<frequency_rbai_model>(m);

        manager.read_corpus_frequencies_file(corpus_frequencies_file);
        manager.read_lemmatization_map(lemmatization_file);
        manager.read_stopwords(stopwords_file);

        json::JSON algo_return = manager.run_algo(analyze_text, return_num_concepts, false);

        /*
        json params = json();
        params.add_attr("term_length", concept_length, false);
        params.add_attr("max_num_concepts", return_num_concepts, false);
        algo_return.add_attr("params", params.get_string(), false);
        */
        cout << algo_return << endl;

    }

    return 0;
}



