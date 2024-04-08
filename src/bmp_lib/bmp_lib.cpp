#include "bmp_lib.h"

#define BMP_CHECK_ERROR_(clause, msg)                                                   \
            if (clause) {                                                               \
                fprintf(stderr, "Error opening BMP file '%s': " msg "\n", filename);   \
                return Status::FILE_ERROR;                                              \
            }

Status::Statuses BmpImg::read_from_file(const char* filename) {
    assert(filename);

    long size = 0;
    STATUS_CHECK(file_open_read_close(filename, &file_buffer, &size, 32 * 2 + 32)); //< extra 64 bytes for AVX
                                                                                    //< + 32 for aligning

    BmpFileHeader* file_header = (BmpFileHeader*)file_buffer;
    BmpInfoHeader* info_header = (BmpInfoHeader*)(file_buffer + sizeof(BmpFileHeader));

    BMP_CHECK_ERROR_(file_header->type != 'B' + ('M' << 8), "wrong signature");
    BMP_CHECK_ERROR_((long)file_header->size != size,       "wrong file size");
    BMP_CHECK_ERROR_(info_header->compression != 0 &&
                     info_header->compression != 3,         "program supports uncompressed files only");
    BMP_CHECK_ERROR_(info_header->bit_count != 32,          "color scheme must be BGRA (32 bits)");

    width = info_header->width;
    height = info_header->height;

#ifdef ALIGN_IMG
    uint32_t old_offset = file_header->offset;

    file_header->offset += 32 - (uint32_t)(((size_t)file_buffer + file_header->offset) % 32);

    memmove(file_buffer + file_header->offset, file_buffer + old_offset, size - old_offset);
#endif //< #ifdef ALIGN_IMG

    img_array = file_buffer + file_header->offset;

    file_size = (size_t)size;

    return Status::NORMAL_WORK;
};
#undef BMP_CHECK_ERROR_

Status::Statuses BmpImg::write_to_file(const char* filename) {
    assert(filename);
    assert(file_buffer);
    assert(file_size > 0);

    STATUS_CHECK(file_open_write_bytes_close(filename, file_buffer, file_size));

    return Status::NORMAL_WORK;
}
