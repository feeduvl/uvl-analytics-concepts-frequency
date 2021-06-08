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

#include "../util/str_util.h"
#include "../io/json.h"

using namespace std;

/**
 * Read in the frequency file and store the contents in an accessible format
 */
template <class model_type>
class frequency_manager{
    string frequencies_filename;
    string lemmatization_filename;
    string stopwords_filename;

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

    vector<string> input_concepts_sorted;
    vector<unsigned int> input_frequencies;
    vector<string> stopwords_sorted;

    vector<int> position_mapping_input_to_corpus;

    unsigned int total_words_corpus = 0;  // first line of the frequencies file
    unsigned int distinct_tokens_corpus = 0;  // first line of the frequencies file
    unsigned int total_concepts_corpus = 0;  // first line of the frequencies file

    vector<double> log_likelihoods_input;


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

            if(str_util::hasEnding(token, "'s")){
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
            if(!is_numeric && !token.empty() && (it == stopwords_sorted.end() || *it != token)){
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
                //cout << "Got stop word: " << token << endl;
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

    model_depth_wrapper<model_type> model_wrapper;


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

    void recurse_find_concepts_and_frequencies(concept_node<string> * node,
                                                      const string & parent_concept, bool first) {
        for (concept_node <string> *c : node->children) {
            string next_parent;
            unsigned int frequency = c->concept_frequency;

            if (first) {
                next_parent = c->getKey();
            } else {
                next_parent = parent_concept + " " + c->getKey();
            }
            input_concepts_sorted.push_back(next_parent);
            input_frequencies.push_back(frequency);
            int corpus_index = get_index_in_corpus(next_parent);
            position_mapping_input_to_corpus.push_back(corpus_index);

            recurse_find_concepts_and_frequencies(c, next_parent, false);
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

public:
    explicit frequency_manager(model_type & model) : model_wrapper(model_depth_wrapper<model_type>(model)){
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
        }
        readFile.close();
        //print_contents();

        //string n = "not";
        //cout << "Concept frequency of 'not' (== '" << corpus_concepts_sorted[get_index_in_corpus(n)] << "') : " << to_string(corpus_frequencies[get_index_in_corpus(n)]) << endl;
    }

    void read_lemmatization_map(string & lemmatization_filename){
        this->lemmatization_filename = lemmatization_filename;
        //cout << "Reading lemmatization file: " << lemmatization_filename << endl;
        ifstream readFile(this->lemmatization_filename);
        string line;
        while (getline (readFile, line)) {
            if(line.empty()){ // last line
                break;
            }
            unsigned int space_at = line.find(' ');
            string base = line.substr(0, space_at);
            string variant = line.substr(space_at+1, line.length());
            lemma_map[variant] = base;
        }
        //cout << "Lemma of 'notations': " << (*lemma_map.find("notations")).second<< endl;
    }

    void read_stopwords(string & stopwords_filename){
        this->stopwords_filename = stopwords_filename;

        //cout << "Reading stopwords file: " << this->stopwords_filename << endl;
        ifstream readFile(this->stopwords_filename);
        string word;
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
        }
    }

    //  we assume that at the beginning, log_likelihoods_input contains w_d + w_c at every position
    double compute_log_likelihood(double w_d, double w_c, double n_d, double n_c){

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
    void recurse_find_concepts_and_frequencies(){
        recurse_find_concepts_and_frequencies(model_wrapper.m.getRootNode(),"", true);
        for(int i = 0; i < log_likelihoods_input.size(); i++){
            log_likelihoods_input[i] = compute_log_likelihood((double)input_frequencies[i],
                                                              (double)position_mapping_input_to_corpus[i]==-1?0 : corpus_frequencies[position_mapping_input_to_corpus[i]],
                                                              (double)total_words_corpus,
                                                              (double) model_wrapper.m.total_words_into_model);

        }
    }

    json run_algo(string & analyze_file, int & return_num_concepts, bool from_file){

        //cout << "Analyzing file: " << analyze_file << endl;

        string content;
        if(from_file){
            std::ifstream ifs(analyze_file);
            content = string( (std::istreambuf_iterator<char>(ifs) ),
                                 (std::istreambuf_iterator<char>()    ) );
        } else {
            content = analyze_file;
        }

        string sentence_delimiters = "\n.:!?\0";
        string word_delimiters = ",—-\"”“ "+sentence_delimiters;
        vector<vector<string>> tokens = tokenize(content, const_cast<char *>(sentence_delimiters.c_str()),
                                                 const_cast<char *>(word_delimiters.c_str()));
        for(const auto& sentence : tokens){
            this->model_wrapper.reset_sentence();
            for (const string& token: sentence) {
                //cout << "Writing token to model: " << token << endl;
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

        //cout << "Computing frequency scores for input text" << endl;
        recurse_find_concepts_and_frequencies();

        for(int i = 0; i < input_frequencies.size(); i++){
            double w_d = input_frequencies[i];
            double w_c = position_mapping_input_to_corpus[i] == -1 ? 0: corpus_frequencies[position_mapping_input_to_corpus[i]];
            //cout << "Computing ll for word: " << input_concepts_sorted[i] << endl;

            double ll = compute_log_likelihood(w_d,
                                               w_c,
                                               (double)model_wrapper.m.total_words_into_model,
                                               (double)total_words_corpus);

            log_likelihoods_input.push_back(ll);
        }

        //cout << "Most likely "<<return_num_concepts<< " words: " << endl;

        json j = json();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
        );
        /*
        j.add_attr("started_at", ms.count(), false);
        j.add_attr("method", "frequency_rbai", true);
        */
        vector<size_t> ranking = sort_indexes(log_likelihoods_input);
        vector<string> sorted_tokens;
        vector<double> scores;

        json topics = json();

        for(int i = 0; i < min(return_num_concepts, (int) ranking.size()); ++i){
            sorted_tokens.push_back(input_concepts_sorted[ranking[ranking.size()-1-i]]);
            scores.push_back(log_likelihoods_input[ranking[ranking.size()-1-i]]);
        }
        topics.add_list("concepts", sorted_tokens);
        topics.add_list("scores", scores);

        j.add_attr("topics", topics.get_string(), false);

        return j;
    }


};

#endif //FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_MANAGER_H
