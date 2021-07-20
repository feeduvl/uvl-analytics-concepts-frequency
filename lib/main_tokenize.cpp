//
// Created by Jakob Weichselbaumer on 15.07.2021.
//
#include <vector>
#include <string>
#include <iostream>
#include "src/io/json.h"
#include "src/model/model.h"
#include "src/model_builder/algorithm_runner.h"

using namespace std;
/**
 * Compute the lemmatizations and tokenizations for all input values
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char** argv) {
    vector<string> lemmatize_these;
    lemmatize_these.reserve(argc-1);

    for(int i = 0; i < argc - 1; i++){
        lemmatize_these.emplace_back(argv[1+i]);
    }

    json::JSON out;
    out["concepts"] = json::Array();


    frequency_model model = frequency_model(1);
    algorithm_runner manager = algorithm_runner(model);

    vector<vector<string>> concepts = manager.lemmatize_tokens(lemmatize_these);

    int i = 0;
    for(auto & concept_ : concepts){
        out["concepts"][i] = json::Array();

        int j = 0;
        for(string & lemma : concept_){
            out["concepts"][i][j++] = lemma;
        }
        i++;
    }
    cout << out << endl;
}