//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_DIRECTORY_WALKER_H
#define FEED_UVL_FINDING_COMPARATIVELY_DIRECTORY_WALKER_H
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <filesystem>
#include "includes/rapidxml/rapidxml.hpp"
#include "src/model/file_accepter.h"

using namespace std;

template <class M> class directory_walker{
public:
    explicit directory_walker(const string& directory_path, const file_accepter<M> & f_a, const M & m ) {
        for(const auto & entry : filesystem::directory_iterator(directory_path)){
            if(entry.is_directory()){
                directory_walker<M>(entry.path().string(), f_a, m);
            } else {
                if(f_a.file_is_acceptable(entry)){
                    cout << "Processing: " << entry.path().string() << endl;
                    try{
                        f_a.process_file(entry, m);
                    } catch(exception & e){
                        cerr << e.what() << endl;
                    }
                }
            }
        }
    }
};


#endif //FEED_UVL_FINDING_COMPARATIVELY_DIRECTORY_WALKER_H
