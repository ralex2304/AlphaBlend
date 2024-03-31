#include "file.h"

Status::Statuses file_open_read_close(const char* filename, char** buf, long* size, size_t extra_size) {
    assert(filename);
    assert(buf);
    assert(*buf == nullptr);

    FILE* file = nullptr;
    if (!file_open(&file, filename, "rb"))
        return Status::FILE_ERROR;

    long file_len = file_get_len(file);
    if (file_len < 0) {
        file_close(file);
        return Status::FILE_ERROR;
    }

    if (size != nullptr)
        *size = file_len;

    *buf = (char*)calloc(file_len + 1 + extra_size, sizeof(char));
    if (*buf == nullptr) {
        printf("Memory alloc error\n");
        file_close(file);
        return Status::MEMORY_EXCEED;
    }

    if (!file_read(file, *buf, file_len)) {
        file_close(file);
        return Status::FILE_ERROR;
    }

    if (!file_close(file))
        return Status::FILE_ERROR;

    return Status::NORMAL_WORK;
}

Status::Statuses file_open_write_bytes_close(const char* filename, char* buf, const size_t size) {
    assert(filename);
    assert(buf);

    FILE* file = nullptr;

    if (!file_open(&file, filename, "wb"))
        return Status::FILE_ERROR;

    if (!file_write_bytes(file, buf, size))
        return Status::FILE_ERROR;

    if (!file_close(file))
        return Status::FILE_ERROR;

    return Status::NORMAL_WORK;
}

bool file_write_bytes(FILE* file, const void* data, size_t len) {
    assert(file);
    assert(data);

    if (fwrite(data, 1, len, file) != len) {
        perror("File write error");
        return false;
    }

    return true;
}

bool file_open(FILE** file, const char* filename, const char* mode) {
    assert(file);
    assert(filename);

    *file = fopen(filename, mode);
    if (*file == nullptr || ferror(*file)) {
        perror("Error opening text file");
        return false;
    }
    return true;
}

long file_get_len(FILE* file) {
    assert(file);

    long file_len = -1;

    if (fseek(file, 0, SEEK_END) != 0 ||
        (file_len = ftell(file)) == -1L ||
        fseek(file, 0, SEEK_SET) != 0) {

            perror("File len detection error");
            return -1;
    }

    return file_len;
}

bool file_read(FILE* file, char* buf, long file_len) {
    assert(file);
    assert(buf);

    size_t readed = fread(buf, sizeof(char), file_len, file);
    if (readed != (size_t)file_len || ferror(file)) {
        perror("File read error");
        return false;
    }

    return true;
}

bool file_close(FILE* file) {
    if (fclose(file) != 0) {
        perror("File close error");
        return false;
    }
    return true;
}

int file_printf(FILE* file, const char* format, ...){
    assert(file);
    assert(format);

    va_list args = {};
    va_start(args, format);

    int res = vfprintf(file, format, args);

    va_end(args);

    if (res == EOF)
        perror("File write error");

    return res;
}
