//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_ACCEPTER_RBAI_H
#define FEED_UVL_FINDING_COMPARATIVELY_ACCEPTER_RBAI_H
#include <iostream>
#include <string>
#include "../../includes/rapidxml/rapidxml.hpp"
#include "../model/file_accepter.h"
#include "../model_builder/xml_reader.h"
#include "../model/frequency_rbai_model.h"
#include "../model/model_depth_wrapper.h"
#include "../util/str_util.h"

using namespace std;

class accepter_rbai : public file_accepter<frequency_rbai_model> {
public:
    [[nodiscard]] bool file_is_acceptable(const filesystem::directory_entry & entry) const override {
        return !entry.is_directory() && str_util::hasEnding(entry.path().string(), ".xml");
    }

    static void word_search(xml_node<> * word_container, model_depth_wrapper<frequency_rbai_model> & model){
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
                    } else {  // make a list of omitted words for use when querying the model

                        /*
                        xml_attribute<> * hw = maybe_word_node->first_attribute("hw");
                        string pos_tag = pos->value();
                        if(pos_tag.find('-') == std::string::npos    // - denotes ambiguity, not a stop word
                                && hw){
                            string word = hw->value();
                            model.m.omit_word(word);
                            //cout << "Omitting word: '" << word << "' of type: '" << pos->value() << endl;
                        }*/

                    }
                }
            }
        }
    }

    static void recursive_sentence_search(const string& sentence_container_tagname, const xml_node<> * parent_node, model_depth_wrapper<frequency_rbai_model> & model){
        for(xml_node<> * sentence_container = parent_node->first_node(sentence_container_tagname.c_str()); sentence_container; sentence_container = sentence_container->next_sibling()){
            recursive_sentence_search(s_container_div, sentence_container, model);
            recursive_sentence_search(s_container_head, sentence_container, model);
            recursive_sentence_search(s_container_p, sentence_container, model);

            if(sentence_container_tagname == s_container_p && sentence_container->first_attribute("type")){
                continue;  // skip non-text paragraphs
            }
            for(xml_node<> * sentence_node = sentence_container->first_node("s"); sentence_node; sentence_node = sentence_node->next_sibling()){
                word_search(sentence_node, model);
                model.reset_sentence();
            }
        }
    }

    void process_file(const filesystem::directory_entry &entry, const frequency_rbai_model & model) const override {
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
            cout << "Didn't find written text node in document: "<< filepath<< endl;
        } else {
            model_depth_wrapper modelDepthWrapper = model_depth_wrapper(model);
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

};


string accepter_rbai::s_container_div = "div";
string accepter_rbai::s_container_p = "p";
string accepter_rbai::s_container_head = "head";

string accepter_rbai::w_container_hi = "hi";  // highlights
string accepter_rbai::w_container_mw = "mw";  // multi-words
string accepter_rbai::word_tag = "w";

vector<string> accepter_rbai::allowed_word_tags = {"AJ0", "AJC", "AJS", "AV0", "ITJ", "NN0", "NN1", "NN2",
                                                           "NP0", "ORD","VM0", "VVB", "VVD", "VVG", "VVI", "VVN", "VVZ", "XX0", "ZZ0" };
#endif //FEED_UVL_FINDING_COMPARATIVELY_ACCEPTER_RBAI_H
