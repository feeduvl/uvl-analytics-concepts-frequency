//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_CORPUS_PARSER_H
#define FEED_UVL_FINDING_COMPARATIVELY_CORPUS_PARSER_H

#include <filesystem>

using namespace std;
/**
 * Virtual class to abstract the "process corpus file" step in frequency_model pretraining/algorithm execution
 */
template <class m> class corpus_parser{
public:
    /**
     * Check if this file is acceptable for use in the algorithm
     * @param entry
     * @return
     */
    [[nodiscard]] virtual bool file_is_acceptable(const filesystem::directory_entry & entry) const = 0;
    /**
     * Process the file after it has been accepted
     * @param entry
     * @param model
     */
    virtual void process_file(const filesystem::directory_entry & entry, const m & model) const = 0;
};


#endif //FEED_UVL_FINDING_COMPARATIVELY_CORPUS_PARSER_H
