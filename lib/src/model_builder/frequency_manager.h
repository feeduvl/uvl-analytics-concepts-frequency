//
// Created by Jakob Weichselbaumer on 15.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MANAGER_H
#define FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MANAGER_H

#include <utility>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#include <numeric>
#include <chrono>
#include <algorithm>

#include "../model/model_depth_wrapper.h"
#include "../model/frequency_model.h"
#include "../graph/concept_node.h"
#include "../util/str_util.h"
#include "../io/json.h"

using namespace std;

/**
 * Read in the frequency file and store the contents in an accessible format
 */
class frequency_manager{
    model_depth_wrapper<frequency_model> model_wrapper;

    string frequencies_filename;
    string lemmatization_filename;
    string stopwords_filename;

    vector<vector<string>> tokenized_input;

    static int get_frequency_pos(const string & line, unsigned int skip_spaces){
        int found_spaces = 0;
        for(int i = 0; i < line.length(); i++){
            if(line[i] == ' '){
                if(++found_spaces == skip_spaces){
                    return i +1;
                }
            }
        }
    }
    static void get_int(const char **p, unsigned int & x) {
        while (**p != '\0') {
            if(**p == ' '){
                ++(*p);
                return;
            }
            x = (x*10) + (**p - '0');
            ++(*p);
        }
    }
public:

    map<string, string> lemma_map = map<string, string>();

    vector<string> corpus_concepts_sorted;
    vector<unsigned int> corpus_frequencies;

    vector<string> input_concept_strings_sorted;
    vector<vector<string>> input_concept_vectors_sorted;
    vector<unsigned int> input_frequencies;
    vector<string> stopwords_sorted;

    vector<int> position_mapping_input_to_corpus;

    unsigned int total_words_corpus = 0;  // first line of the frequencies file
    unsigned int distinct_tokens_corpus = 0;  // first line of the frequencies file
    unsigned int total_concepts_corpus = 0;  // first line of the frequencies file

private:
    void print_corpus_contents(){
        for (int i = 0; i < corpus_concepts_sorted.size(); i++) {
            cout << "Concept: '" << corpus_concepts_sorted[i] << "' Frequency: " << to_string(corpus_frequencies[i]) << endl;
        }
    }


    std::vector<std::string> tokenize_sentence(const std::string& source, char* word_delimiters)
    {
        std::string value = source;
        std::vector<std::string> tokens;
        int delimeterIndex = value.find_first_of(word_delimiters);
        while (true)
        {
            std::string token = value.substr(0, delimeterIndex);
            std::for_each(token.begin(), token.end(), [](char & c){  //  everything to lower case
                c = ::tolower(c);
            });

            if(str_util::hasEnding(token, "'s") || str_util::hasEnding(token, "’s")){
                token = token.substr(0, token.size()-2);
                //cout << "Stripped possessive apostrophe: 's, yielding word: " << token << endl;
            }
            ++model_wrapper.m.total_words_read;

            bool is_numeric = true;
            try{
                stoi(token);
            } catch (exception & e){
                is_numeric = false;
            }

            const auto it = lower_bound(stopwords_sorted.begin(), stopwords_sorted.end(), token);
            if(!is_numeric && token.size() >= 3 && (it == stopwords_sorted.end() || *it != token)){
                //cout << "Got token: " << token << endl;
                ++model_wrapper.m.total_words_into_model;
                auto iter = lemma_map.find(token);
                if(iter!=lemma_map.end()){
                    //cout << "Lemma for '" << token << "' : " << iter->second << endl;
                    tokens.push_back(iter->second);
                } else {
                    //cout << "Got unknown token OR base lemma: " << token << endl;
                    tokens.push_back(token);
                }
            } else {
                //cerr << "Stopword in input: '" << token << '\'' <<endl;
            }

            value = value.substr(delimeterIndex + 1);
            if(delimeterIndex == std::string::npos){
                break;
            }
            delimeterIndex = value.find_first_of(word_delimiters);
        }
        return tokens;
    }

    std::vector<vector<std::string>> tokenize(const std::string& source, char * sentence_delimiters, char * word_delimiters)
    {
        vector<vector<string>> sentences = vector<vector<string>>();

        std::string value = source;
        std::vector<std::string> tokens;
        int delimeterIndex = value.find_first_of(sentence_delimiters);
        bool done = false;
        while (true)
        {
            if(delimeterIndex == std::string::npos){
                done = true;
                delimeterIndex = value.size();
            }
            std::string whole_sentence = value.substr(0, delimeterIndex);
            sentences.push_back(tokenize_sentence(whole_sentence, word_delimiters));
            if(done){
                break;
            }
            value = value.substr(delimeterIndex + 1);
            delimeterIndex = value.find_first_of(sentence_delimiters);
        }
        return sentences;
    }

    int get_index_in_corpus(string & c) {
        const auto it = lower_bound(corpus_concepts_sorted.begin(), corpus_concepts_sorted.end(), c);
        if(it == corpus_concepts_sorted.end() || *it != c) {  // concept not found
            //cout << "Concept not found in corpus: " << c << endl;
            return -1;
        } else {
            int idx = it - corpus_concepts_sorted.begin();
            return idx;
        }
    }

    void recurse_find_vector_concepts_and_frequencies(concept_node<string> * node,
                                                      const vector<string> & parent_concept,
                                                      bool first) {
        for (concept_node <string> *c : node->children) {
            vector<string> next_parent;
            unsigned int frequency = c->concept_frequency;

            if (first) {
                next_parent = {c->getKey()};
            } else {
                next_parent = vector<string>(parent_concept);
                next_parent.push_back(c->getKey());
            }
            input_concept_vectors_sorted.push_back(next_parent);
            input_frequencies.push_back(frequency);
            string next_parent_string = str_util::vector_to_string(next_parent);
            int corpus_index = get_index_in_corpus(next_parent_string);
            position_mapping_input_to_corpus.push_back(corpus_index);
            c->corpus_index = corpus_index;

            recurse_find_vector_concepts_and_frequencies(c, next_parent, false);
        }
    }

    void recurse_find_string_concepts_and_frequencies(concept_node<string> * node,
                                                      const string & parent_concept,
                                                      bool first) {
        for (concept_node <string> *c : node->children) {
            string next_parent;
            unsigned int frequency = c->concept_frequency;

            if (first) {
                next_parent = c->getKey();
            } else {
                next_parent = parent_concept + " " + c->getKey();
            }
            input_concept_strings_sorted.push_back(next_parent);
            input_frequencies.push_back(frequency);
            int corpus_index = get_index_in_corpus(next_parent);
            position_mapping_input_to_corpus.push_back(corpus_index);
            c->corpus_index = corpus_index;

            recurse_find_string_concepts_and_frequencies(c, next_parent, false);
        }
    }
    template <typename T>
    vector<size_t> sort_indexes(const vector<T> &v) {

        // initialize original index locations
        vector<size_t> idx(v.size());
        iota(idx.begin(), idx.end(), 0);

        // sort indexes based on comparing values in v
        // using std::stable_sort instead of std::sort
        // to avoid unnecessary index re-orderings
        // when v contains elements of equal values
        stable_sort(idx.begin(), idx.end(),
                    [&v](size_t i1, size_t i2) {return v[i1] < v[i2];});

        return idx;
    }

    vector<double> w1 = {1};
    vector<double> w2 = {0.6, 0.4};
    vector<double> w3 = {0.5, 0.3, 0.2};


public:
    explicit frequency_manager(frequency_model & model): model_wrapper(model_depth_wrapper<frequency_model>(model)){

    }

    void read_corpus_frequencies_file(const string& frequencies_filename){
        this->frequencies_filename = frequencies_filename;
        //cout << "Reading corpus frequencies file: " << frequencies_filename << endl;
        ifstream readFile(this->frequencies_filename);
        string line;
        getline(readFile, line);
        const char * arr = line.c_str();
        get_int(&arr, this->total_words_corpus);
        get_int(&arr, this->distinct_tokens_corpus);
        get_int(&arr, this->total_concepts_corpus);
        //cout << "Total words read into model: " << this->total_words_corpus << endl;
        //cout << "Distinct tokens: " << this->distinct_tokens_corpus << endl;
        //cout << "Number of concepts: " << this->total_concepts_corpus << endl;
        unsigned int lines_read = 0;
        while (getline (readFile, line)) {
            if(line.empty()){  // last line
                break;
            }

            arr = line.c_str();
            unsigned int depth = 0;
            get_int(&arr, depth);
            int frequency_pos = get_frequency_pos(line, depth+1);
            unsigned int frequency = 0;
            string numeral = line.substr(frequency_pos, 10);
            arr = numeral.c_str();
            get_int(&arr, frequency);
            unsigned int skip = to_string(depth).length()+1;
            string c = line.substr(skip, frequency_pos-1 -skip );
            corpus_frequencies.push_back(frequency);
            corpus_concepts_sorted.push_back(c);
            lines_read++;
        }
        readFile.close();
        //print_contents();

        if(lines_read > 0){
            //string n = "not";
            //cerr << "Concept frequency of 'not' (== '" << corpus_concepts_sorted[get_index_in_corpus(n)] << "') : " << to_string(corpus_frequencies[get_index_in_corpus(n)]) << endl;
        } else {
            cerr << "Got empty frequencies file!" << endl;
        }
    }

    void read_lemmatization_map(string & lemmatization_filename){
        this->lemmatization_filename = lemmatization_filename;
        //cout << "Reading lemmatization file: " << lemmatization_filename << endl;
        ifstream readFile(this->lemmatization_filename);
        string line;
        unsigned int lines_read = 0;
        while (getline (readFile, line)) {
            if(line.empty()){ // last line
                break;
            }
            unsigned int space_at = line.find(' ');
            string base = line.substr(0, space_at);
            string variant = line.substr(space_at+1, line.length());
            lemma_map[variant] = base;
            lines_read++;
        }
        if(lines_read==0){
            cerr << "Got empty lemmatization file!" << endl;
        }
        //cerr << "Lemma of 'notations': " << (*lemma_map.find("notations")).second<< endl;
    }

    void read_stopwords(string & stopwords_filename){
        this->stopwords_filename = stopwords_filename;

        ifstream readFile(this->stopwords_filename);
        string word;
        unsigned int lines_read = 0;
        while (getline (readFile, word)) {
            if(word.empty()){ // last line
                break;
            }
            const auto it = lower_bound(stopwords_sorted.begin(), stopwords_sorted.end(), word);
            if(it == stopwords_sorted.end()){
                stopwords_sorted.push_back(word);
            } else {
                if(*it != word){
                    stopwords_sorted.insert(it, word);
                }
            }
            lines_read++;
        }
        if(lines_read==0){
            cerr << "Got empty stopwords file!" << endl;
        }
    }
    vector<double> & get_relative_token_weight(size_t term_length){

        if(term_length == 1){
            return w1;
        } else if (term_length ==2){
            return w2;
        } else if (term_length == 3){
            return w3;
        } else {
            cerr << "Got unsupported term length: "<<term_length<<", exiting." << endl;
            exit(1);
        }
    }

    double compute_term_log_likelihood(vector<string> & term, double n_d, double n_c){
        double total_score = 0;
        vector<double> & weight = get_relative_token_weight(term.size());
        size_t idx = 0;
        for(auto & t : term){
            vector<string> singleton = {t};
            concept_node<string> * token_node = model_wrapper.m.getRootNode()->get_node(singleton);

            //cout << "Singleton: " << vector_to_string(singleton) << endl;

            //cout << "Frequency: " << token_node->concept_frequency << " equivalent corpus entry: " << (token_node->corpus_index==-1?"(not found)" : corpus_concepts_sorted[token_node->corpus_index]) << endl;

            total_score += weight[idx]*compute_log_likelihood(token_node->concept_frequency,
                                                  token_node->corpus_index == -1 ? 0 : corpus_frequencies[token_node->corpus_index],
                                                  n_d,
                                                  n_c);
            idx++;
        }
        return total_score;
    }

    static double compute_log_likelihood(double w_d, double w_c, double n_d, double n_c){

        double E_d, E_c;
        E_d = (n_d * (w_d+w_c))/(n_c + n_d);
        E_c = (n_c * (w_d+w_c))/(n_c + n_d);
        double log_likelihood = w_d * log(w_d/E_d);
        if(w_c!=0){
            log_likelihood += w_c * log(w_c/E_c);
        }
        log_likelihood = 2*log_likelihood;
        //cout << "n_c: " << n_c << " n_d: " << n_d << " w_c: " << w_c << " w_d: " << w_d << " result: " << log_likelihood << endl;
        return log_likelihood;
    }
    void recurse_find_string_concepts_and_frequencies(){
        recurse_find_string_concepts_and_frequencies(model_wrapper.m.getRootNode(), "", true);
    }
    void recurse_find_vector_concepts_and_frequencies(){
        recurse_find_vector_concepts_and_frequencies(model_wrapper.m.getRootNode(), vector<string>(), true);
    }

private:

    const string sentence_delimiters = "\n.;:!?”““„〟„()[]{}&$%+#*~<>|/\0";
    const string word_delimiters = " ,—-\t\"'‘’0123456789"+sentence_delimiters;

    void get_tokens(string & analyze_this, bool from_file, bool save_tokenization){

        string content;
        if(from_file){
            std::ifstream ifs(analyze_this);
            content = string( (std::istreambuf_iterator<char>(ifs) ),
                              (std::istreambuf_iterator<char>()    ) );
        } else {
            content = analyze_this;
        }

        vector<vector<string>> tokens = tokenize(content, const_cast<char *>(sentence_delimiters.c_str()),
                                                 const_cast<char *>(word_delimiters.c_str()));
        if(save_tokenization){
            this->tokenized_input = tokens;
        }
        for(const auto& sentence : tokens){
            this->model_wrapper.reset_sentence();
            for (const string& token: sentence) {
                bool was_inserted;
                unsigned int upper;
                if(model_wrapper.pipeline_loaded){
                    upper = model_wrapper.m.getTermLength();
                } else{
                    upper = model_wrapper.current_word_index + 1;
                }
                for(int i = 0; i < upper; i++){
                    model_wrapper.list[i] = model_wrapper.list[i]->add_child(token, was_inserted, model_wrapper.m);
                    if(was_inserted){
                        model_wrapper.m.concepts++;
                    }
                }
                model_wrapper.next_word();
            }
        }
    }

    static double get_relevance_fcic(double F_corpus, double F_input){
        return F_input / (F_corpus + F_input);
    }

public:

    /**
     * Returns a list representing the number of occurences for each concept
     * @param analyze_this
     * @param concept_length
     * @param key_lemmas
     * @return
     */

    vector<unsigned int> find_occurences(string & analyze_this, const unsigned int & concept_length, vector<string> & key_lemmas) {

        vector<unsigned int> occurences = vector<unsigned int>(key_lemmas.size(), 0);
        vector<vector<string>> tokens = tokenize(analyze_this, const_cast<char *>(sentence_delimiters.c_str()),
                                                 const_cast<char *>(word_delimiters.c_str()));

        vector<string> pipeline;
        vector<string> split_word;

        for(vector<string> & s : tokens){
            pipeline.clear();
            for(string & t : s){
                if(pipeline.size()>=concept_length){
                    pipeline.pop_back();
                }
                pipeline.insert(pipeline.begin(), t);

                for(int i = 0; i < key_lemmas.size(); i++){
                    split_word.clear();
                    str_util::split(key_lemmas[i], split_word, ' ');
                    std::reverse(split_word.begin(), split_word.end());
                    if(split_word==vector<string>(pipeline.begin(), pipeline.begin()+split_word.size())){
                        occurences[i]++;
                        break;
                    }
                }
            }
        }

        return occurences;
    }

    json::JSON run_rbai(string & analyze_this, int & return_num_concepts, bool from_file){
        get_tokens(analyze_this, from_file, false);
        recurse_find_vector_concepts_and_frequencies();

        vector<double> log_likelihoods_input;
        auto n_d = (double)model_wrapper.m.total_words_into_model;
        auto n_c = (double)total_words_corpus;

        for(int i = 0; i < input_frequencies.size(); i++){

            double ll = compute_term_log_likelihood(input_concept_vectors_sorted[i], n_d, n_c);
            log_likelihoods_input.push_back(ll);
        }
        json::JSON j;

        vector<size_t> ranking = sort_indexes(log_likelihoods_input);

        j["topics"] = json::Object();
        j["topics"]["concepts"] = json::Array();
        j["topics"]["scores"] = json::Array();

        for(int i = 0; i < min(return_num_concepts, (int) ranking.size()); ++i){
            j["topics"]["concepts"][i] = str_util::vector_to_string(input_concept_vectors_sorted[ranking[ranking.size() - 1 - i]]);
            j["topics"]["scores"][i] = log_likelihoods_input[ranking[ranking.size()-1-i]];
        }
        return j;
    }

    const frequency_model & get_model(){
        return model_wrapper.m;
    }
    vector<string> run_fcic(string & analyze_this, int & return_num_concepts, bool from_file){
        get_tokens(analyze_this, from_file, true);

        recurse_find_string_concepts_and_frequencies();  // we can use "..._string_..." here because we limit this algorithm to concepts of length 1

        vector<double> frequency_scores_input;
        vector<string> candidate_concepts;

        frequency_scores_input = vector<double>(input_frequencies.size(), 0);
        for(int i = 0; i < input_frequencies.size(); i++){
            frequency_scores_input[i] = get_relevance_fcic( position_mapping_input_to_corpus[i] == -1 ? 0: corpus_frequencies[position_mapping_input_to_corpus[i]],
                                                                 input_frequencies[i]);
        }

        vector<size_t> ranking = sort_indexes(frequency_scores_input);

        for(int i = 0; i < min(return_num_concepts, (int) ranking.size()); ++i){
            candidate_concepts.push_back(input_concept_strings_sorted[ranking[ranking.size() - 1 - i]]);
        }

        get_model().setCandidateTokensDecTree(candidate_concepts);
        for(auto & sentence : tokenized_input){
            vector<bool> s = get_model().get_empty_sentence(candidate_concepts);
            for(auto & token : sentence){
                get_model().process_token_dec_tree(s, token);
            }
            get_model().add_dec_tree_sentence(s, true);
        }

        return candidate_concepts;
    }


};

#endif //FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MANAGER_H
