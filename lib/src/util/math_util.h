//
// Created by Jakob Weichselbaumer on 25.06.2021.
//

#ifndef FEED_UVL_FINDING_COMPARATIVELY_MATH_UTIL_H
#define FEED_UVL_FINDING_COMPARATIVELY_MATH_UTIL_H

unsigned int intPow(unsigned int x, unsigned int p)
{
    if (p == 0) return 1;
    if (p == 1) return x;

    int tmp = intPow(x, p / 2);
    if (p%2 == 0) return tmp * tmp;
    else return x * tmp * tmp;
}

#endif //FEED_UVL_FINDING_COMPARATIVELY_MATH_UTIL_H
