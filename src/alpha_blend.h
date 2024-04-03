#ifndef ALPHA_BLEND_H_
#define ALPHA_BLEND_H_

#include <immintrin.h>
#include <x86intrin.h>

#include "config.h"
#include "bmp_lib/bmp_lib.h"
#include "utils/statuses.h"

Status::Statuses alpha_blend(BmpImg* background, BmpImg* foreground);

#endif //< #ifndef ALPHA_BLEND_H_
