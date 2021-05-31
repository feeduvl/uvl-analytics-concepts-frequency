//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_XML_READER_H
#define FEED_UVL_FINDING_COMPARATIVELY_XML_READER_H
#include <string>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include "rapidxml.hpp"

using namespace rapidxml;
using namespace std;

class xml_reader {
    string filePath;

public:
    explicit xml_reader(string filePath) : filePath(std::move(filePath)) {}

    [[nodiscard]] const xml_document<> * getDoc() const {
        auto * doc = new xml_document<>();
        // Read the xml file into a vector
        ifstream theFile (this->filePath);

        vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
        buffer.push_back('\0');
        // Parse the buffer using the xml file parsing library into doc
        doc->parse<0>(&buffer[0]);
        return doc;
    }
};

#endif //FEED_UVL_FINDING_COMPARATIVELY_XML_READER_H
