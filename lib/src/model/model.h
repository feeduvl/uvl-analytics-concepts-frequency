//
// Created by Jakob Weichselbaumer on 12.05.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_MODEL_H
#define FEED_UVL_FINDING_COMPARATIVELY_MODEL_H

template <class T> class model {
public:

    const unsigned int term_length;
    mutable unsigned int l1_words = 0;
    mutable unsigned int total_words_read = 0;
    mutable unsigned int total_words_into_model = 0;
    mutable unsigned int concepts = 0;
    mutable unsigned int highest_frequency = 0;

    mutable unsigned int frequency_updates = 0;
    mutable T most_common_term;

    [[nodiscard]] unsigned int getTermLength() const {
        return term_length;
    }

    explicit model(unsigned int t_l) : term_length(t_l){}
};

#endif //FEED_UVL_FINDING_COMPARATIVELY_MODEL_H
