#pragma once

#include <zlib.h>
#include <stdint.h>

void *gz_decompress (const void *compressed, int compressed_len, uint64_t *decompressed_lenp); 
