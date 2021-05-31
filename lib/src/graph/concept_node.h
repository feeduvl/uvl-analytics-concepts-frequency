//
// Created by Jakob Weichselbaumer on 10.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_CONCEPT_NODE_H
#define FEED_UVL_FINDING_COMPARATIVELY_CONCEPT_NODE_H

#include <vector>
#include <algorithm>
#include <string>
#include "src/model/model.h"
using namespace std;

/**
 * T is key type
 */
template <class T> struct concept_node{
    int depth{};
    T * key;

    mutable unsigned int concept_frequency{};
    vector<concept_node<T> *> children = vector<concept_node*>();

    concept_node(T * key, int depth){
        this->depth = depth;
        this->key = key;
        this->concept_frequency = 1;
    }

    T & getKey() const {
        return *key;
    }

    void incrementFrequency(){
        ++concept_frequency;
    }

    /**
     * This should be called on the root node
     * @return nullptr if concept not found, else the concept's leaf node
     */
     /*
    concept_node * get_node(vector<T> & terms){
        T & word = terms[terms.size() - 1];
        auto * ret_node = new concept_node<T>(child_p, this->depth + 1);
        const auto it = lower_bound(children.begin(), children.end(), ret_node, [](concept_node<T>*a, concept_node<T>*b){return a->getKey() < b->getKey();});
        delete ret_node;
        if(it == children.end() || **it->getKey()!=word){
            return nullptr;  // word not found
        }
        if(terms.size()==1){
            return *it;
        } else {
            terms.pop_back();
            return get_node(terms);
        }
    }*/

    /**
     * @param child key for the new child
     * @return returns a reference to the new child node
     */
    concept_node * add_child(const T & child, bool & was_inserted, const model<T> & m){
        T * child_p = new T(child);
        auto * ret_node = new concept_node<T>(child_p, this->depth + 1);
        const auto it = lower_bound(children.begin(), children.end(), ret_node, [](concept_node<T>*a, concept_node<T>*b){return a->getKey() < b->getKey();});

        if(it == children.end()){
            //cout << "Inserted concept: "<< child << " at depth: " <<depth << " as last child." << endl;

            //cout << "Inserted concept: "<< child << " at depth: " <<depth+1 << " at position: " << (it - children.begin()) << endl;
            children.push_back(ret_node);
            was_inserted = true;
        } else {
            concept_node<T> * node = *it;
            if(node->getKey() == child){
                delete ret_node;
                node->incrementFrequency();
                //cout << "Incremented frequency of concept: "<< child << " at depth " <<depth+1 << endl;
                int index = (it - children.begin());
                //cout << "New frequency: " << node->concept_frequency << endl;
                ret_node = node;
                was_inserted = false;
                if(node->concept_frequency > m.highest_frequency){
                    m.highest_frequency = node->concept_frequency;
                    m.most_common_term = node->getKey();
                }
            } else {
                //cout << "Inserted concept: "<< child << " at depth: " <<depth+1 << " at position: " << (it - children.begin()) << endl;
                children.insert(it, ret_node);
                was_inserted = true;
            }
        }
        if(was_inserted && this->depth==0){  // we have an l1-word a.k.a a new token
            m.l1_words++;
        }
        return ret_node;
    }

};

#endif //FEED_UVL_FINDING_COMPARATIVELY_CONCEPT_NODE_H
