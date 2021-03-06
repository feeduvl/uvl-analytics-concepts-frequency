//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_CORPUS_PARSER_H
#define FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_CORPUS_PARSER_H
#include <iostream>
#include <string>
#include "../../includes/rapidxml/rapidxml.hpp"
#include "../model/corpus_parser.h"
#include "../model_builder/xml_reader.h"
#include "../model/frequency_model.h"
#include "../model/pipeline.h"
#include "../util/str_util.h"

using namespace std;

class frequency_corpus_parser : public corpus_parser<frequency_model> {
public:
    [[nodiscard]] bool file_is_acceptable(const filesystem::directory_entry & entry) const override {
        return !entry.is_directory() && str_util::hasEnding(entry.path().string(), ".xml");
    }

    static void word_search(xml_node<> * word_container, pipeline<frequency_model> & model){
        for(xml_node<> * maybe_word_node = word_container->first_node(); maybe_word_node; maybe_word_node = maybe_word_node->next_sibling()){
            if(maybe_word_node->name()==w_container_hi || maybe_word_node->name()==w_container_mw){  // highlighted word or multi-word
                word_search(maybe_word_node, model);
            } else if(maybe_word_node->name()==word_tag){  // word node
                xml_attribute<> * pos = maybe_word_node->first_attribute("c5");
                if(pos){
                    model.m.total_words_read++;
                    if(model.m.total_words_read%200000 == 0){
                        cout << "Total concepts: " << model.m.concepts << endl;
                        cout << "Total words read or ignored: " << model.m.total_words_read << endl;
                        cout << "Words accepted into model: " << model.m.total_words_into_model << endl;
                        cout << "Distinct tokens:  " << model.m.l1_words << endl;

                        //model.m.recurse_print_concepts();
                    }

                    if(find(allowed_word_tags.begin(), allowed_word_tags.end(), pos->value())!=allowed_word_tags.end()){
                        //  word is allowed, get the headword
                        xml_attribute<> * hw = maybe_word_node->first_attribute("hw");
                        if(hw){
                            //  process headword
                            model.m.total_words_into_model++;
                            bool was_inserted;
                            unsigned int upper;
                            if(model.pipeline_loaded){
                                upper = model.m.getTermLength();
                            } else{
                                upper = model.current_word_index+1;
                            }
                            for(int i = 0; i < upper; i++){
                                model.list[i] = model.list[i]->add_child(hw->value(), was_inserted, model.m);
                                if(was_inserted){
                                    model.m.concepts++;
                                }
                            }
                            model.next_word();

                        }
                    }
                }
            }
        }
    }

    static void decision_tree_word_search(xml_node<> * word_container, pipeline<frequency_model> & model, vector<bool> & sentence) {
        for (xml_node<> *maybe_word_node = word_container->first_node(); maybe_word_node; maybe_word_node = maybe_word_node->next_sibling()) {
            if (maybe_word_node->name() == w_container_hi ||
                maybe_word_node->name() == w_container_mw) {  // highlighted word or multi-word
                decision_tree_word_search(maybe_word_node, model, sentence);
            } else if (maybe_word_node->name() == word_tag) {  // word node
                xml_attribute<> *pos = maybe_word_node->first_attribute("c5");
                if (pos) {
                    if (find(allowed_word_tags.begin(), allowed_word_tags.end(), pos->value()) !=
                        allowed_word_tags.end()) {
                        //  word is allowed, get the headword
                        xml_attribute<> *hw = maybe_word_node->first_attribute("hw");
                        if (hw) {
                            model.m.process_token_dec_tree(sentence, hw->value());
                        }
                    }
                }
            }
        }
    }

    void recursive_sentence_search(const string& sentence_container_tagname, const xml_node<> * parent_node, pipeline<frequency_model> & model) const{
        for(xml_node<> * sentence_container = parent_node->first_node(sentence_container_tagname.c_str()); sentence_container; sentence_container = sentence_container->next_sibling()){
            recursive_sentence_search(s_container_div, sentence_container, model);
            recursive_sentence_search(s_container_head, sentence_container, model);
            recursive_sentence_search(s_container_p, sentence_container, model);

            if(sentence_container_tagname == s_container_p && sentence_container->first_attribute("type")){
                continue;  // skip non-text paragraphs
            }
            for(xml_node<> * sentence_node = sentence_container->first_node("s"); sentence_node; sentence_node = sentence_node->next_sibling()){
                if(this->mode==Mode::TRAIN_CORPUS){
                    word_search(sentence_node, model);
                    model.reset_sentence();
                } else if(this->mode == Mode::DEC_TREE){
                    vector<bool> sentence = model.m.get_empty_sentence();
                    decision_tree_word_search(sentence_node, model, sentence);
                    model.m.add_dec_tree_sentence(sentence, false);
                }

            }
        }
    }

    void process_file(const filesystem::directory_entry &entry, const frequency_model & model) const override {
        string filepath = entry.path().string();

        xml_document<> doc;
        // Read the xml file into a vector
        ifstream theFile (entry.path().string());

        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        // Parse the buffer using the xml file parsing library into doc
        doc.parse<0>(&buffer[0]);
        xml_node<> * root_node = doc.first_node("bncDoc");
        xml_node<> * text_node = root_node->first_node("wtext");
        if(text_node == nullptr){
            // cout << "Didn't find written text node in document: "<< filepath<< endl;
        } else {
            pipeline modelDepthWrapper = pipeline(model);
            recursive_sentence_search(s_container_div, text_node, modelDepthWrapper);
            recursive_sentence_search(s_container_head, text_node, modelDepthWrapper);
            recursive_sentence_search(s_container_p, text_node, modelDepthWrapper);
        }
        theFile.close();
        doc.clear();
    }
public:
    static vector<string> allowed_word_tags;

    //  the following types of tags may contain sentences
    static string s_container_div;
    static string s_container_head;
    static string s_container_p;

    // the following types of tags may contain words
    static string w_container_mw;
    static string w_container_hi;

    // the tag indicating a word
    static string word_tag;

    enum Mode{TRAIN_CORPUS, DEC_TREE}; // an enum for type of run
    explicit frequency_corpus_parser(Mode mode);
private:
    Mode mode;

};

frequency_corpus_parser::frequency_corpus_parser(Mode mode) {
    this->mode = mode;
}

string frequency_corpus_parser::s_container_div = "div";
string frequency_corpus_parser::s_container_p = "p";
string frequency_corpus_parser::s_container_head = "head";

string frequency_corpus_parser::w_container_hi = "hi";  // highlights
string frequency_corpus_parser::w_container_mw = "mw";  // multi-words
string frequency_corpus_parser::word_tag = "w";

vector<string> frequency_corpus_parser::allowed_word_tags = {"AJ0", "AJC", "AJS", "AV0", "ITJ", "NN0", "NN1", "NN2",
                                                           "NP0", "ORD","VM0", "VVB", "VVD", "VVG", "VVI", "VVN", "VVZ", "XX0", "ZZ0" };


#endif //FEED_UVL_FINDING_COMPARATIVELY_FREQUENCY_CORPUS_PARSER_H
