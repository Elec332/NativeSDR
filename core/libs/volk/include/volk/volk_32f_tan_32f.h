/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*!
 * \page volk_32f_tan_32f
 *
 * \b Overview
 *
 * Computes the tangent of each element of the aVector.
 *
 * b[i] = tan(a[i])
 *
 * <b>Dispatcher Prototype</b>
 * \code
 * void volk_32f_tan_32f(float* bVector, const float* aVector, unsigned int num_points)
 * \endcode
 *
 * \b Inputs
 * \li aVector: The buffer of points.
 * \li num_points: The number of values in input buffer.
 *
 * \b Outputs
 * \li bVector: The output buffer.
 *
 * \b Example
 * Calculate tan(theta) for common angles.
 * \code
 *   int N = 10;
 *   unsigned int alignment = volk_get_alignment();
 *   float* in = (float*)volk_malloc(sizeof(float)*N, alignment);
 *   float* out = (float*)volk_malloc(sizeof(float)*N, alignment);
 *
 *   in[0] = 0.000;
 *   in[1] = 0.524;
 *   in[2] = 0.785;
 *   in[3] = 1.047;
 *   in[4] = 1.571  ;
 *   in[5] = 1.571  ;
 *   in[6] = -1.047;
 *   in[7] = -0.785;
 *   in[8] = -0.524;
 *   in[9] = -0.000;
 *
 *   volk_32f_tan_32f(out, in, N);
 *
 *   for(unsigned int ii = 0; ii < N; ++ii){
 *       printf("tan(%1.3f) = %1.3f\n", in[ii], out[ii]);
 *   }
 *
 *   volk_free(in);
 *   volk_free(out);
 * \endcode
 */

#include <inttypes.h>
#include <math.h>
#include <stdio.h>

#ifndef INCLUDED_volk_32f_tan_32f_a_H
#define INCLUDED_volk_32f_tan_32f_a_H

#if LV_HAVE_AVX2 && LV_HAVE_FMA
#include <immintrin.h>

static inline void
volk_32f_tan_32f_a_avx2_fma(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int eighthPoints = num_points / 8;
    unsigned int i = 0;

    __m256 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones,
        fzeroes;
    __m256 sine, cosine, tangent, condition1, condition2, condition3;
    __m256i q, r, ones, twos, fours;

    m4pi = _mm256_set1_ps(1.273239545);
    pio4A = _mm256_set1_ps(0.78515625);
    pio4B = _mm256_set1_ps(0.241876e-3);
    ffours = _mm256_set1_ps(4.0);
    ftwos = _mm256_set1_ps(2.0);
    fones = _mm256_set1_ps(1.0);
    fzeroes = _mm256_setzero_ps();
    ones = _mm256_set1_epi32(1);
    twos = _mm256_set1_epi32(2);
    fours = _mm256_set1_epi32(4);

    cp1 = _mm256_set1_ps(1.0);
    cp2 = _mm256_set1_ps(0.83333333e-1);
    cp3 = _mm256_set1_ps(0.2777778e-2);
    cp4 = _mm256_set1_ps(0.49603e-4);
    cp5 = _mm256_set1_ps(0.551e-6);

    for (; number < eighthPoints; number++) {
        aVal = _mm256_load_ps(aPtr);
        s = _mm256_sub_ps(aVal,
                          _mm256_and_ps(_mm256_mul_ps(aVal, ftwos),
                                        _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS)));
        q = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_mul_ps(s, m4pi)));
        r = _mm256_add_epi32(q, _mm256_and_si256(q, ones));

        s = _mm256_fnmadd_ps(_mm256_cvtepi32_ps(r), pio4A, s);
        s = _mm256_fnmadd_ps(_mm256_cvtepi32_ps(r), pio4B, s);

        s = _mm256_div_ps(
            s,
            _mm256_set1_ps(8.0)); // The constant is 2^N, for 3 times argument reduction
        s = _mm256_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm256_mul_ps(
            _mm256_fmadd_ps(
                _mm256_fmsub_ps(
                    _mm256_fmadd_ps(_mm256_fmsub_ps(s, cp5, cp4), s, cp3), s, cp2),
                s,
                cp1),
            s);

        for (i = 0; i < 3; i++) {
            s = _mm256_mul_ps(s, _mm256_sub_ps(ffours, s));
        }
        s = _mm256_div_ps(s, ftwos);

        sine = _mm256_sqrt_ps(_mm256_mul_ps(_mm256_sub_ps(ftwos, s), s));
        cosine = _mm256_sub_ps(fones, s);

        condition1 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, ones), twos)),
            fzeroes,
            _CMP_NEQ_UQ);
        condition2 = _mm256_cmp_ps(
            _mm256_cmp_ps(
                _mm256_cvtepi32_ps(_mm256_and_si256(q, fours)), fzeroes, _CMP_NEQ_UQ),
            _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS),
            _CMP_NEQ_UQ);
        condition3 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, twos), fours)),
            fzeroes,
            _CMP_NEQ_UQ);

        __m256 temp = cosine;
        cosine =
            _mm256_add_ps(cosine, _mm256_and_ps(_mm256_sub_ps(sine, cosine), condition1));
        sine = _mm256_add_ps(sine, _mm256_and_ps(_mm256_sub_ps(temp, sine), condition1));
        sine = _mm256_sub_ps(
            sine, _mm256_and_ps(_mm256_mul_ps(sine, _mm256_set1_ps(2.0f)), condition2));
        cosine = _mm256_sub_ps(
            cosine,
            _mm256_and_ps(_mm256_mul_ps(cosine, _mm256_set1_ps(2.0f)), condition3));
        tangent = _mm256_div_ps(sine, cosine);
        _mm256_store_ps(bPtr, tangent);
        aPtr += 8;
        bPtr += 8;
    }

    number = eighthPoints * 8;
    for (; number < num_points; number++) {
        *bPtr++ = tan(*aPtr++);
    }
}

#endif /* LV_HAVE_AVX2 && LV_HAVE_FMA for aligned */

#ifdef LV_HAVE_AVX2
#include <immintrin.h>

static inline void
volk_32f_tan_32f_a_avx2(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int eighthPoints = num_points / 8;
    unsigned int i = 0;

    __m256 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones,
        fzeroes;
    __m256 sine, cosine, tangent, condition1, condition2, condition3;
    __m256i q, r, ones, twos, fours;

    m4pi = _mm256_set1_ps(1.273239545);
    pio4A = _mm256_set1_ps(0.78515625);
    pio4B = _mm256_set1_ps(0.241876e-3);
    ffours = _mm256_set1_ps(4.0);
    ftwos = _mm256_set1_ps(2.0);
    fones = _mm256_set1_ps(1.0);
    fzeroes = _mm256_setzero_ps();
    ones = _mm256_set1_epi32(1);
    twos = _mm256_set1_epi32(2);
    fours = _mm256_set1_epi32(4);

    cp1 = _mm256_set1_ps(1.0);
    cp2 = _mm256_set1_ps(0.83333333e-1);
    cp3 = _mm256_set1_ps(0.2777778e-2);
    cp4 = _mm256_set1_ps(0.49603e-4);
    cp5 = _mm256_set1_ps(0.551e-6);

    for (; number < eighthPoints; number++) {
        aVal = _mm256_load_ps(aPtr);
        s = _mm256_sub_ps(aVal,
                          _mm256_and_ps(_mm256_mul_ps(aVal, ftwos),
                                        _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS)));
        q = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_mul_ps(s, m4pi)));
        r = _mm256_add_epi32(q, _mm256_and_si256(q, ones));

        s = _mm256_sub_ps(s, _mm256_mul_ps(_mm256_cvtepi32_ps(r), pio4A));
        s = _mm256_sub_ps(s, _mm256_mul_ps(_mm256_cvtepi32_ps(r), pio4B));

        s = _mm256_div_ps(
            s,
            _mm256_set1_ps(8.0)); // The constant is 2^N, for 3 times argument reduction
        s = _mm256_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm256_mul_ps(
            _mm256_add_ps(
                _mm256_mul_ps(
                    _mm256_sub_ps(
                        _mm256_mul_ps(
                            _mm256_add_ps(
                                _mm256_mul_ps(_mm256_sub_ps(_mm256_mul_ps(s, cp5), cp4),
                                              s),
                                cp3),
                            s),
                        cp2),
                    s),
                cp1),
            s);

        for (i = 0; i < 3; i++) {
            s = _mm256_mul_ps(s, _mm256_sub_ps(ffours, s));
        }
        s = _mm256_div_ps(s, ftwos);

        sine = _mm256_sqrt_ps(_mm256_mul_ps(_mm256_sub_ps(ftwos, s), s));
        cosine = _mm256_sub_ps(fones, s);

        condition1 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, ones), twos)),
            fzeroes,
            _CMP_NEQ_UQ);
        condition2 = _mm256_cmp_ps(
            _mm256_cmp_ps(
                _mm256_cvtepi32_ps(_mm256_and_si256(q, fours)), fzeroes, _CMP_NEQ_UQ),
            _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS),
            _CMP_NEQ_UQ);
        condition3 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, twos), fours)),
            fzeroes,
            _CMP_NEQ_UQ);

        __m256 temp = cosine;
        cosine =
            _mm256_add_ps(cosine, _mm256_and_ps(_mm256_sub_ps(sine, cosine), condition1));
        sine = _mm256_add_ps(sine, _mm256_and_ps(_mm256_sub_ps(temp, sine), condition1));
        sine = _mm256_sub_ps(
            sine, _mm256_and_ps(_mm256_mul_ps(sine, _mm256_set1_ps(2.0f)), condition2));
        cosine = _mm256_sub_ps(
            cosine,
            _mm256_and_ps(_mm256_mul_ps(cosine, _mm256_set1_ps(2.0f)), condition3));
        tangent = _mm256_div_ps(sine, cosine);
        _mm256_store_ps(bPtr, tangent);
        aPtr += 8;
        bPtr += 8;
    }

    number = eighthPoints * 8;
    for (; number < num_points; number++) {
        *bPtr++ = tan(*aPtr++);
    }
}

#endif /* LV_HAVE_AVX2 for aligned */

#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void
volk_32f_tan_32f_a_sse4_1(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int quarterPoints = num_points / 4;
    unsigned int i = 0;

    __m128 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones,
        fzeroes;
    __m128 sine, cosine, tangent, condition1, condition2, condition3;
    __m128i q, r, ones, twos, fours;

    m4pi = _mm_set1_ps(1.273239545);
    pio4A = _mm_set1_ps(0.78515625);
    pio4B = _mm_set1_ps(0.241876e-3);
    ffours = _mm_set1_ps(4.0);
    ftwos = _mm_set1_ps(2.0);
    fones = _mm_set1_ps(1.0);
    fzeroes = _mm_setzero_ps();
    ones = _mm_set1_epi32(1);
    twos = _mm_set1_epi32(2);
    fours = _mm_set1_epi32(4);

    cp1 = _mm_set1_ps(1.0);
    cp2 = _mm_set1_ps(0.83333333e-1);
    cp3 = _mm_set1_ps(0.2777778e-2);
    cp4 = _mm_set1_ps(0.49603e-4);
    cp5 = _mm_set1_ps(0.551e-6);

    for (; number < quarterPoints; number++) {
        aVal = _mm_load_ps(aPtr);
        s = _mm_sub_ps(aVal,
                       _mm_and_ps(_mm_mul_ps(aVal, ftwos), _mm_cmplt_ps(aVal, fzeroes)));
        q = _mm_cvtps_epi32(_mm_floor_ps(_mm_mul_ps(s, m4pi)));
        r = _mm_add_epi32(q, _mm_and_si128(q, ones));

        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4A));
        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4B));

        s = _mm_div_ps(
            s, _mm_set1_ps(8.0)); // The constant is 2^N, for 3 times argument reduction
        s = _mm_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm_mul_ps(
            _mm_add_ps(
                _mm_mul_ps(
                    _mm_sub_ps(
                        _mm_mul_ps(
                            _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_mul_ps(s, cp5), cp4), s),
                                       cp3),
                            s),
                        cp2),
                    s),
                cp1),
            s);

        for (i = 0; i < 3; i++) {
            s = _mm_mul_ps(s, _mm_sub_ps(ffours, s));
        }
        s = _mm_div_ps(s, ftwos);

        sine = _mm_sqrt_ps(_mm_mul_ps(_mm_sub_ps(ftwos, s), s));
        cosine = _mm_sub_ps(fones, s);

        condition1 = _mm_cmpneq_ps(
            _mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, ones), twos)), fzeroes);
        condition2 = _mm_cmpneq_ps(
            _mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(q, fours)), fzeroes),
            _mm_cmplt_ps(aVal, fzeroes));
        condition3 = _mm_cmpneq_ps(
            _mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, twos), fours)), fzeroes);

        __m128 temp = cosine;
        cosine = _mm_add_ps(cosine, _mm_and_ps(_mm_sub_ps(sine, cosine), condition1));
        sine = _mm_add_ps(sine, _mm_and_ps(_mm_sub_ps(temp, sine), condition1));
        sine =
            _mm_sub_ps(sine, _mm_and_ps(_mm_mul_ps(sine, _mm_set1_ps(2.0f)), condition2));
        cosine = _mm_sub_ps(
            cosine, _mm_and_ps(_mm_mul_ps(cosine, _mm_set1_ps(2.0f)), condition3));
        tangent = _mm_div_ps(sine, cosine);
        _mm_store_ps(bPtr, tangent);
        aPtr += 4;
        bPtr += 4;
    }

    number = quarterPoints * 4;
    for (; number < num_points; number++) {
        *bPtr++ = tanf(*aPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for aligned */


#endif /* INCLUDED_volk_32f_tan_32f_a_H */

#ifndef INCLUDED_volk_32f_tan_32f_u_H
#define INCLUDED_volk_32f_tan_32f_u_H

#if LV_HAVE_AVX2 && LV_HAVE_FMA
#include <immintrin.h>

static inline void
volk_32f_tan_32f_u_avx2_fma(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int eighthPoints = num_points / 8;
    unsigned int i = 0;

    __m256 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones,
        fzeroes;
    __m256 sine, cosine, tangent, condition1, condition2, condition3;
    __m256i q, r, ones, twos, fours;

    m4pi = _mm256_set1_ps(1.273239545);
    pio4A = _mm256_set1_ps(0.78515625);
    pio4B = _mm256_set1_ps(0.241876e-3);
    ffours = _mm256_set1_ps(4.0);
    ftwos = _mm256_set1_ps(2.0);
    fones = _mm256_set1_ps(1.0);
    fzeroes = _mm256_setzero_ps();
    ones = _mm256_set1_epi32(1);
    twos = _mm256_set1_epi32(2);
    fours = _mm256_set1_epi32(4);

    cp1 = _mm256_set1_ps(1.0);
    cp2 = _mm256_set1_ps(0.83333333e-1);
    cp3 = _mm256_set1_ps(0.2777778e-2);
    cp4 = _mm256_set1_ps(0.49603e-4);
    cp5 = _mm256_set1_ps(0.551e-6);

    for (; number < eighthPoints; number++) {
        aVal = _mm256_loadu_ps(aPtr);
        s = _mm256_sub_ps(aVal,
                          _mm256_and_ps(_mm256_mul_ps(aVal, ftwos),
                                        _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS)));
        q = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_mul_ps(s, m4pi)));
        r = _mm256_add_epi32(q, _mm256_and_si256(q, ones));

        s = _mm256_fnmadd_ps(_mm256_cvtepi32_ps(r), pio4A, s);
        s = _mm256_fnmadd_ps(_mm256_cvtepi32_ps(r), pio4B, s);

        s = _mm256_div_ps(
            s,
            _mm256_set1_ps(8.0)); // The constant is 2^N, for 3 times argument reduction
        s = _mm256_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm256_mul_ps(
            _mm256_fmadd_ps(
                _mm256_fmsub_ps(
                    _mm256_fmadd_ps(_mm256_fmsub_ps(s, cp5, cp4), s, cp3), s, cp2),
                s,
                cp1),
            s);

        for (i = 0; i < 3; i++) {
            s = _mm256_mul_ps(s, _mm256_sub_ps(ffours, s));
        }
        s = _mm256_div_ps(s, ftwos);

        sine = _mm256_sqrt_ps(_mm256_mul_ps(_mm256_sub_ps(ftwos, s), s));
        cosine = _mm256_sub_ps(fones, s);

        condition1 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, ones), twos)),
            fzeroes,
            _CMP_NEQ_UQ);
        condition2 = _mm256_cmp_ps(
            _mm256_cmp_ps(
                _mm256_cvtepi32_ps(_mm256_and_si256(q, fours)), fzeroes, _CMP_NEQ_UQ),
            _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS),
            _CMP_NEQ_UQ);
        condition3 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, twos), fours)),
            fzeroes,
            _CMP_NEQ_UQ);

        __m256 temp = cosine;
        cosine =
            _mm256_add_ps(cosine, _mm256_and_ps(_mm256_sub_ps(sine, cosine), condition1));
        sine = _mm256_add_ps(sine, _mm256_and_ps(_mm256_sub_ps(temp, sine), condition1));
        sine = _mm256_sub_ps(
            sine, _mm256_and_ps(_mm256_mul_ps(sine, _mm256_set1_ps(2.0f)), condition2));
        cosine = _mm256_sub_ps(
            cosine,
            _mm256_and_ps(_mm256_mul_ps(cosine, _mm256_set1_ps(2.0f)), condition3));
        tangent = _mm256_div_ps(sine, cosine);
        _mm256_storeu_ps(bPtr, tangent);
        aPtr += 8;
        bPtr += 8;
    }

    number = eighthPoints * 8;
    for (; number < num_points; number++) {
        *bPtr++ = tan(*aPtr++);
    }
}

#endif /* LV_HAVE_AVX2 && LV_HAVE_FMA for unaligned */

#ifdef LV_HAVE_AVX2
#include <immintrin.h>

static inline void
volk_32f_tan_32f_u_avx2(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int eighthPoints = num_points / 8;
    unsigned int i = 0;

    __m256 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones,
        fzeroes;
    __m256 sine, cosine, tangent, condition1, condition2, condition3;
    __m256i q, r, ones, twos, fours;

    m4pi = _mm256_set1_ps(1.273239545);
    pio4A = _mm256_set1_ps(0.78515625);
    pio4B = _mm256_set1_ps(0.241876e-3);
    ffours = _mm256_set1_ps(4.0);
    ftwos = _mm256_set1_ps(2.0);
    fones = _mm256_set1_ps(1.0);
    fzeroes = _mm256_setzero_ps();
    ones = _mm256_set1_epi32(1);
    twos = _mm256_set1_epi32(2);
    fours = _mm256_set1_epi32(4);

    cp1 = _mm256_set1_ps(1.0);
    cp2 = _mm256_set1_ps(0.83333333e-1);
    cp3 = _mm256_set1_ps(0.2777778e-2);
    cp4 = _mm256_set1_ps(0.49603e-4);
    cp5 = _mm256_set1_ps(0.551e-6);

    for (; number < eighthPoints; number++) {
        aVal = _mm256_loadu_ps(aPtr);
        s = _mm256_sub_ps(aVal,
                          _mm256_and_ps(_mm256_mul_ps(aVal, ftwos),
                                        _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS)));
        q = _mm256_cvtps_epi32(_mm256_floor_ps(_mm256_mul_ps(s, m4pi)));
        r = _mm256_add_epi32(q, _mm256_and_si256(q, ones));

        s = _mm256_sub_ps(s, _mm256_mul_ps(_mm256_cvtepi32_ps(r), pio4A));
        s = _mm256_sub_ps(s, _mm256_mul_ps(_mm256_cvtepi32_ps(r), pio4B));

        s = _mm256_div_ps(
            s,
            _mm256_set1_ps(8.0)); // The constant is 2^N, for 3 times argument reduction
        s = _mm256_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm256_mul_ps(
            _mm256_add_ps(
                _mm256_mul_ps(
                    _mm256_sub_ps(
                        _mm256_mul_ps(
                            _mm256_add_ps(
                                _mm256_mul_ps(_mm256_sub_ps(_mm256_mul_ps(s, cp5), cp4),
                                              s),
                                cp3),
                            s),
                        cp2),
                    s),
                cp1),
            s);

        for (i = 0; i < 3; i++) {
            s = _mm256_mul_ps(s, _mm256_sub_ps(ffours, s));
        }
        s = _mm256_div_ps(s, ftwos);

        sine = _mm256_sqrt_ps(_mm256_mul_ps(_mm256_sub_ps(ftwos, s), s));
        cosine = _mm256_sub_ps(fones, s);

        condition1 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, ones), twos)),
            fzeroes,
            _CMP_NEQ_UQ);
        condition2 = _mm256_cmp_ps(
            _mm256_cmp_ps(
                _mm256_cvtepi32_ps(_mm256_and_si256(q, fours)), fzeroes, _CMP_NEQ_UQ),
            _mm256_cmp_ps(aVal, fzeroes, _CMP_LT_OS),
            _CMP_NEQ_UQ);
        condition3 = _mm256_cmp_ps(
            _mm256_cvtepi32_ps(_mm256_and_si256(_mm256_add_epi32(q, twos), fours)),
            fzeroes,
            _CMP_NEQ_UQ);

        __m256 temp = cosine;
        cosine =
            _mm256_add_ps(cosine, _mm256_and_ps(_mm256_sub_ps(sine, cosine), condition1));
        sine = _mm256_add_ps(sine, _mm256_and_ps(_mm256_sub_ps(temp, sine), condition1));
        sine = _mm256_sub_ps(
            sine, _mm256_and_ps(_mm256_mul_ps(sine, _mm256_set1_ps(2.0f)), condition2));
        cosine = _mm256_sub_ps(
            cosine,
            _mm256_and_ps(_mm256_mul_ps(cosine, _mm256_set1_ps(2.0f)), condition3));
        tangent = _mm256_div_ps(sine, cosine);
        _mm256_storeu_ps(bPtr, tangent);
        aPtr += 8;
        bPtr += 8;
    }

    number = eighthPoints * 8;
    for (; number < num_points; number++) {
        *bPtr++ = tan(*aPtr++);
    }
}

#endif /* LV_HAVE_AVX2 for unaligned */


#ifdef LV_HAVE_SSE4_1
#include <smmintrin.h>

static inline void
volk_32f_tan_32f_u_sse4_1(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;

    unsigned int number = 0;
    unsigned int quarterPoints = num_points / 4;
    unsigned int i = 0;

    __m128 aVal, s, m4pi, pio4A, pio4B, cp1, cp2, cp3, cp4, cp5, ffours, ftwos, fones,
        fzeroes;
    __m128 sine, cosine, tangent, condition1, condition2, condition3;
    __m128i q, r, ones, twos, fours;

    m4pi = _mm_set1_ps(1.273239545);
    pio4A = _mm_set1_ps(0.78515625);
    pio4B = _mm_set1_ps(0.241876e-3);
    ffours = _mm_set1_ps(4.0);
    ftwos = _mm_set1_ps(2.0);
    fones = _mm_set1_ps(1.0);
    fzeroes = _mm_setzero_ps();
    ones = _mm_set1_epi32(1);
    twos = _mm_set1_epi32(2);
    fours = _mm_set1_epi32(4);

    cp1 = _mm_set1_ps(1.0);
    cp2 = _mm_set1_ps(0.83333333e-1);
    cp3 = _mm_set1_ps(0.2777778e-2);
    cp4 = _mm_set1_ps(0.49603e-4);
    cp5 = _mm_set1_ps(0.551e-6);

    for (; number < quarterPoints; number++) {
        aVal = _mm_loadu_ps(aPtr);
        s = _mm_sub_ps(aVal,
                       _mm_and_ps(_mm_mul_ps(aVal, ftwos), _mm_cmplt_ps(aVal, fzeroes)));
        q = _mm_cvtps_epi32(_mm_floor_ps(_mm_mul_ps(s, m4pi)));
        r = _mm_add_epi32(q, _mm_and_si128(q, ones));

        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4A));
        s = _mm_sub_ps(s, _mm_mul_ps(_mm_cvtepi32_ps(r), pio4B));

        s = _mm_div_ps(
            s, _mm_set1_ps(8.0)); // The constant is 2^N, for 3 times argument reduction
        s = _mm_mul_ps(s, s);
        // Evaluate Taylor series
        s = _mm_mul_ps(
            _mm_add_ps(
                _mm_mul_ps(
                    _mm_sub_ps(
                        _mm_mul_ps(
                            _mm_add_ps(_mm_mul_ps(_mm_sub_ps(_mm_mul_ps(s, cp5), cp4), s),
                                       cp3),
                            s),
                        cp2),
                    s),
                cp1),
            s);

        for (i = 0; i < 3; i++) {
            s = _mm_mul_ps(s, _mm_sub_ps(ffours, s));
        }
        s = _mm_div_ps(s, ftwos);

        sine = _mm_sqrt_ps(_mm_mul_ps(_mm_sub_ps(ftwos, s), s));
        cosine = _mm_sub_ps(fones, s);

        condition1 = _mm_cmpneq_ps(
            _mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, ones), twos)), fzeroes);
        condition2 = _mm_cmpneq_ps(
            _mm_cmpneq_ps(_mm_cvtepi32_ps(_mm_and_si128(q, fours)), fzeroes),
            _mm_cmplt_ps(aVal, fzeroes));
        condition3 = _mm_cmpneq_ps(
            _mm_cvtepi32_ps(_mm_and_si128(_mm_add_epi32(q, twos), fours)), fzeroes);

        __m128 temp = cosine;
        cosine = _mm_add_ps(cosine, _mm_and_ps(_mm_sub_ps(sine, cosine), condition1));
        sine = _mm_add_ps(sine, _mm_and_ps(_mm_sub_ps(temp, sine), condition1));
        sine =
            _mm_sub_ps(sine, _mm_and_ps(_mm_mul_ps(sine, _mm_set1_ps(2.0f)), condition2));
        cosine = _mm_sub_ps(
            cosine, _mm_and_ps(_mm_mul_ps(cosine, _mm_set1_ps(2.0f)), condition3));
        tangent = _mm_div_ps(sine, cosine);
        _mm_storeu_ps(bPtr, tangent);
        aPtr += 4;
        bPtr += 4;
    }

    number = quarterPoints * 4;
    for (; number < num_points; number++) {
        *bPtr++ = tanf(*aPtr++);
    }
}

#endif /* LV_HAVE_SSE4_1 for unaligned */


#ifdef LV_HAVE_GENERIC

static inline void
volk_32f_tan_32f_generic(float* bVector, const float* aVector, unsigned int num_points)
{
    float* bPtr = bVector;
    const float* aPtr = aVector;
    unsigned int number = 0;

    for (; number < num_points; number++) {
        *bPtr++ = tanf(*aPtr++);
    }
}
#endif /* LV_HAVE_GENERIC */


#ifdef LV_HAVE_NEON
#include <arm_neon.h>
#include <volk/volk_neon_intrinsics.h>

static inline void
volk_32f_tan_32f_neon(float* bVector, const float* aVector, unsigned int num_points)
{
    unsigned int number = 0;
    unsigned int quarter_points = num_points / 4;
    float* bVectorPtr = bVector;
    const float* aVectorPtr = aVector;

    float32x4_t b_vec;
    float32x4_t a_vec;

    for (number = 0; number < quarter_points; number++) {
        a_vec = vld1q_f32(aVectorPtr);
        // Prefetch next one, speeds things up
        __VOLK_PREFETCH(aVectorPtr + 4);
        b_vec = _vtanq_f32(a_vec);
        vst1q_f32(bVectorPtr, b_vec);
        // move pointers ahead
        bVectorPtr += 4;
        aVectorPtr += 4;
    }

    // Deal with the rest
    for (number = quarter_points * 4; number < num_points; number++) {
        *bVectorPtr++ = tanf(*aVectorPtr++);
    }
}
#endif /* LV_HAVE_NEON */


#endif /* INCLUDED_volk_32f_tan_32f_u_H */
