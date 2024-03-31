#ifndef BMP_LIB_H_
#define BMP_LIB_H_

#include <inttypes.h>

#include "../utils/macros.h"
#include "../file/file.h"

struct BmpFileHeader {
    uint16_t type      = 0;
    uint32_t size      = 0;
    uint16_t reserved1 = 0;
    uint16_t reserved2 = 0;
    uint32_t offset    = 0;
} __attribute__((packed));

struct BmpColor {
    uint8_t b = 0;
    uint8_t g = 0;
    uint8_t r = 0;
    uint8_t a = 0;
};

struct BmpInfoHeader {
    uint32_t self_size = sizeof(BmpInfoHeader);
    int32_t width  = 0;
    int32_t height = 0;
    uint16_t planes = 1;
    uint16_t bit_count = 0;
    uint32_t compression = 0;
    uint32_t img_size = 0;
    int32_t X_pixels_per_meter = 0;
    int32_t Y_pixels_per_meter = 0;
    uint32_t colors_used = 0;
    uint32_t colors_important = 0;
};

struct BmpImg {
    ssize_t width  = 0;
    ssize_t height = 0;

    long file_size = 0;

    char* file_buffer = nullptr;
    char* img_array = nullptr;

    ~BmpImg() { FREE(file_buffer); }

    Status::Statuses read_from_file(const char* filename);

    Status::Statuses write_to_file(const char* filename);
};

#endif //< #ifndef BMP_LIB_H_
