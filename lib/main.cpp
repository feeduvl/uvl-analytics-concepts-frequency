#include <iostream>
#include <string>
#include <vector>

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
        json j = json();
        j.add_attr("filename", "testing_no_filename", true);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        );
        j.add_attr("timestamp", ms.count(), false);
        j.add_attr("algo_name", "test", true);
        j.add_attr("concept_length", "-1", false);

        auto sorted_tokens = vector<string>();
        auto scores = vector<double>();
        j.add_list("concepts", sorted_tokens);
        j.add_list("scores", scores);
        cout << j.get_string() << endl;
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
        string corpus_frequencies = argv[4];
        string lemmatization_filename = "res/lemmatization-en.txt";
        string analyze_text = argv[5];
        string stopwords_file = "res/stopwords.txt";
        int return_num_concepts = stoi(argv[6]);

        frequency_rbai_model m = frequency_rbai_model();
        m.setTermLength((unsigned int) stoi(concept_length));
        frequency_manager<frequency_rbai_model> manager = frequency_manager<frequency_rbai_model>(m);

        manager.read_corpus_frequencies_file(corpus_frequencies);
        manager.read_lemmatization_map(lemmatization_filename);
        manager.read_stopwords(stopwords_file);

        json algo_return = manager.run_algo(analyze_text, return_num_concepts, false);
        cout << algo_return.get_string() << endl;

    }

    return 0;
}



