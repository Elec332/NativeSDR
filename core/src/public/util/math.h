//
// Created by Elec332 on 05/11/2021.
//

#ifndef NATIVESDR_MATH_H
#define NATIVESDR_MATH_H

inline size_t floor_divu(size_t a, size_t b) {
    size_t d = a / b;
    size_t r = a % b;  /* optimizes into single division. */
    return r ? (size_t)(d - 1) : d;
}

inline int floor_div(int a, int b) {
    int d = a / b;
    int r = a % b;  /* optimizes into single division. */
    return r ? (d - ((a < 0) ^ (b < 0))) : d;
}

#endif //NATIVESDR_MATH_H
