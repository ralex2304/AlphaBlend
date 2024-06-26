#include "alpha_blend.h"

#ifdef NAIVE
static Status::Statuses alpha_blend_naive_(BmpImg* background, BmpImg* foreground);
#endif

#ifdef VECTOR_AVX
static Status::Statuses alpha_blend_avx_(BmpImg* background, BmpImg* foreground);
#endif

#define MAKE_POSITIVE_(x_) (x_) = (x_) < 0 ? -(x_) : (x_)

Status::Statuses alpha_blend(BmpImg* background, BmpImg* foreground) {
    assert(background);
    assert(foreground);
    assert(background->file_buffer);
    assert(foreground->file_buffer);

    if (background->width * foreground->width < 0 ||
        background->height * foreground->height < 0) {

        fprintf(stderr, "Error: foreground and backgorund images must have same bmp orientation\n");
        return Status::INPUT_ERROR;
    }

    if (background->width < foreground->width ||
        background->height < foreground->height) {

        fprintf(stderr, "Error: foreground must be smaller than background\n");
        return Status::INPUT_ERROR;
    }

    if (background->width != foreground->width) {
        fprintf(stderr, "Error: background and foreground must have equal width\n");
        return Status::INPUT_ERROR;
    }

    MAKE_POSITIVE_(background->width);
    MAKE_POSITIVE_(background->height);
    MAKE_POSITIVE_(foreground->width);
    MAKE_POSITIVE_(foreground->height);

    unsigned long long begin = __rdtsc();

    for (int i = 0; i < MEASURE_ITERS; i++) {

#if defined NAIVE
        STATUS_CHECK(alpha_blend_naive_(background, foreground));
#elif defined VECTOR_AVX
        STATUS_CHECK(alpha_blend_avx_(background, foreground));
#endif

    }

    unsigned long long end = __rdtsc();

    printf("%llu\n", end - begin);

    return Status::NORMAL_WORK;
}
#undef MAKE_POSITIVE_

#ifdef NAIVE
static Status::Statuses alpha_blend_naive_(BmpImg* background, BmpImg* foreground) {
    assert(background);
    assert(foreground);
    assert(background->file_buffer);
    assert(foreground->file_buffer);
    assert(foreground->width <= background->width);
    assert(foreground->height <= background->height);

    BmpColor* back_pixel = (BmpColor*)background->img_array;
    BmpColor* fore_pixel = (BmpColor*)foreground->img_array;

    size_t width_diff = background->width - foreground->width;

    for (ssize_t y = 0; y < foreground->height; y++) {

        for (ssize_t x = 0; x < foreground->width; x++) {

            uint8_t alpha = fore_pixel->a;
            uint8_t rev_alpha = 255 - alpha;

            back_pixel->b = (uint8_t)((fore_pixel->b * alpha + back_pixel->b * rev_alpha) / 255);
            back_pixel->g = (uint8_t)((fore_pixel->g * alpha + back_pixel->g * rev_alpha) / 255);
            back_pixel->r = (uint8_t)((fore_pixel->r * alpha + back_pixel->r * rev_alpha) / 255);

            back_pixel++;
            fore_pixel++;
        }

        back_pixel += width_diff;
        // fore_pixel has been already set as needed
    }

    return Status::NORMAL_WORK;
}
#endif //< #ifdef NAIVE


#ifdef VECTOR_AVX

inline static __m256i _mm256_div255_epi16(__m256i a) {
#ifndef INACCURATE_DIV255
    return _mm256_srli_epi16(_mm256_add_epi16(_mm256_add_epi16 (a, _mm256_set1_epi16(1)),
                                              _mm256_srli_epi16(a, 8)),
                             8);    //< x / 255 = (x + 1 + (x >> 8)) >> 8
#else //< #ifdef INACCURATE_DIV255
    return _mm256_srli_epi16(a, 8); //< x / 256 = x >> 8
#endif //< #ifndef INACCURATE_DIV255
}

inline static __m128i _mm_load_si128_aligned_choice_(__m128i const* a) {
#ifdef ALIGN_IMG
    return _mm_load_si128(a);
#else //< #ifndef ALIGN_IMG
    return _mm_loadu_si128(a);
#endif //< #ifdef ALIGN_IMG
}

inline static void _mm256_store_si256_aligned_choice_(__m256i* p, __m256i a) {
#ifdef ALIGN_IMG
    _mm256_store_si256(p, a);
#else //< #ifndef ALIGN_IMG
    _mm256_storeu_si256(p, a);
#endif //< #ifdef ALIGN_IMG
}

static Status::Statuses alpha_blend_avx_(BmpImg* background, BmpImg* foreground) {
    assert(background);
    assert(foreground);
    assert(background->file_buffer);
    assert(foreground->file_buffer);
    assert(foreground->width <= background->width);
    assert(foreground->height <= background->height);
    assert(foreground->width == background->width); //< aligning

    BmpColor* back_pixel = (BmpColor*)background->img_array;
    BmpColor* fore_pixel = (BmpColor*)foreground->img_array;

    for (ssize_t i = 0; i < foreground->width * foreground->height / (4 * 2); i++) {

        // load pixels as 8-bit and extend them to 16-bit
        __m256i fore1 = _mm256_cvtepu8_epi16(_mm_load_si128_aligned_choice_((__m128i*)(fore_pixel + 0)));

        __m256i fore2 = _mm256_cvtepu8_epi16(_mm_load_si128_aligned_choice_((__m128i*)(fore_pixel + 4)));

        __m256i back1 = _mm256_cvtepu8_epi16(_mm_load_si128_aligned_choice_((__m128i*)(back_pixel + 0)));

        __m256i back2 = _mm256_cvtepu8_epi16(_mm_load_si128_aligned_choice_((__m128i*)(back_pixel + 4)));

        // set alpha channel vector
        __m256i alpha1 =
                _mm256_set_epi16(0xFFFFu, (fore_pixel + 3)->a, (fore_pixel + 3)->a, (fore_pixel + 3)->a,
                                 0xFFFFu, (fore_pixel + 2)->a, (fore_pixel + 2)->a, (fore_pixel + 2)->a,
                                 0xFFFFu, (fore_pixel + 1)->a, (fore_pixel + 1)->a, (fore_pixel + 1)->a,
                                 0xFFFFu, (fore_pixel + 0)->a, (fore_pixel + 0)->a, (fore_pixel + 0)->a);

        __m256i alpha2 =
                _mm256_set_epi16(0xFFFFu, (fore_pixel + 7)->a, (fore_pixel + 7)->a, (fore_pixel + 7)->a,
                                 0xFFFFu, (fore_pixel + 6)->a, (fore_pixel + 6)->a, (fore_pixel + 6)->a,
                                 0xFFFFu, (fore_pixel + 5)->a, (fore_pixel + 5)->a, (fore_pixel + 5)->a,
                                 0xFFFFu, (fore_pixel + 4)->a, (fore_pixel + 4)->a, (fore_pixel + 4)->a);

        // rev_alpha1 = 255 - alpha1
        __m256i rev_alpha1 = _mm256_sub_epi16(_mm256_set1_epi16(255), alpha1);

        __m256i rev_alpha2 = _mm256_sub_epi16(_mm256_set1_epi16(255), alpha2);

        // sum1 = (fore1 * alpha1 + back1 * rev_alpha1)
        __m256i sum1 = _mm256_add_epi16(_mm256_mullo_epi16(fore1, alpha1),
                                        _mm256_mullo_epi16(back1, rev_alpha1));

        __m256i sum2 = _mm256_add_epi16(_mm256_mullo_epi16(fore2, alpha2),
                                        _mm256_mullo_epi16(back2, rev_alpha2));

        // result1 = (fore1 * alpha1 + back1 * rev_alpha1) / 255
        __m256i result1 = _mm256_div255_epi16(sum1);

        __m256i result2 = _mm256_div255_epi16(sum2);

        // result = {result1[left_half], result2[left_half], result1[right_half], result2[right_half]}
        __m256i result = _mm256_packus_epi16(result1, result2);

        // result = {result1, result2}
        result = _mm256_permute4x64_epi64(result, 0b11'01'10'00);

        _mm256_store_si256_aligned_choice_((__m256i*)back_pixel, result);

        back_pixel += 2 * 4; //< 2 packs of 4 pixels each
        fore_pixel += 2 * 4;
    }

    return Status::NORMAL_WORK;
}
#endif //< #ifdef VECTOR_AVX
