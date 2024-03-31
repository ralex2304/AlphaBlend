#include "alpha_blend.h"

inline static Status::Statuses alpha_blend_naive_(BmpImg* background, BmpImg* foreground);

inline static Status::Statuses alpha_blend_avx_(BmpImg* background, BmpImg* foreground);

#define MAKE_POSITIVE_(x_) x_ = x_ < 0 ? -x_ : x_

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

    MAKE_POSITIVE_(background->width);
    MAKE_POSITIVE_(background->height);
    MAKE_POSITIVE_(foreground->width);
    MAKE_POSITIVE_(foreground->height);

#if defined NAIVE
    return alpha_blend_naive_(background, foreground);
#elif defined VECTOR_AVX
    return alpha_blend_avx_(background, foreground);
#endif

    assert(0 && "Must return earlier. Check defines");
}
#undef MAKE_POSITIVE_

inline static Status::Statuses alpha_blend_naive_(BmpImg* background, BmpImg* foreground) {
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

            back_pixel->b = (uint8_t)((fore_pixel->b * alpha + back_pixel->b * (255 - alpha)) / 255);
            back_pixel->g = (uint8_t)((fore_pixel->g * alpha + back_pixel->g * (255 - alpha)) / 255);
            back_pixel->r = (uint8_t)((fore_pixel->r * alpha + back_pixel->r * (255 - alpha)) / 255);

            back_pixel++;
            fore_pixel++;
        }

        back_pixel += width_diff;
        // fore_pixel has been already set as needed
    }

    return Status::NORMAL_WORK;
}

inline static Status::Statuses alpha_blend_avx_(BmpImg* background, BmpImg* foreground) {
    assert(background);
    assert(foreground);
    assert(background->file_buffer);
    assert(foreground->file_buffer);

    // TODO alpha_blend_avx_

    return Status::NORMAL_WORK;
}
