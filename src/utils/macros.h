#ifndef MACROS_H_
#define MACROS_H_
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define FREE(ptr) do {                \
                      free(ptr);      \
                      ptr = nullptr;  \
                  } while (0)

inline void* recalloc(void* ptr, size_t old_size, size_t new_size) {
    ptr = realloc(ptr, new_size);

    if (!ptr)
        return ptr;

    if (new_size > old_size)
        memset((char*)ptr + old_size, 0, new_size - old_size);

    return ptr;
}

inline void fill(void* array, size_t len, const void* elem, const size_t elem_size) {
    while (len--) {
        memcpy(array, elem, elem_size);
        array = (char*)array + elem_size;
    }
}

inline size_t time_microseconds() {

    struct timeval tv = {};
    gettimeofday(&tv, NULL);

    return (size_t)tv.tv_sec * 1000'000 + (size_t)tv.tv_usec;
}

#endif // #ifndef MACROS_H_
