//
// Created by Jakob Weichselbaumer on 11.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_FILE_ACCEPTER_H
#define FEED_UVL_FINDING_COMPARATIVELY_FILE_ACCEPTER_H

#include <filesystem>

using namespace std;
/**
 * Virtual class to abstract the "process corpus file" step in algo_1_model pretraining/algorithm execution
 */
template <class m> class file_accepter{
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


#endif //FEED_UVL_FINDING_COMPARATIVELY_FILE_ACCEPTER_H
