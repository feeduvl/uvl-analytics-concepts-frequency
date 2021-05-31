//
// Created by Jakob Weichselbaumer on 10.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_TERM_H
#define FEED_UVL_FINDING_COMPARATIVELY_TERM_H

#include <vector>
#include <string>
using namespace std;

/**
 * Abstraction of a term of a certain max length, e.g. "lead horse water" (abstracting "lead horse to water", stop word 'to' removed)
 */
struct term {
    static unsigned int size;  // max length, min = 1
    vector<char *> body;  // contains the term
    term(){
        body = vector<char *>(size);
    }
    void add(char * word){
        body.push_back(word);
    }
};
#endif //FEED_UVL_FINDING_COMPARATIVELY_TERM_H
