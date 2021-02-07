#include "Compression.h"
#include <stdlib.h>

void *gz_decompress (const void *compressed, int compressed_len,
			uint64_t *decompressed_lenp) {
	int pass = 1;
	char *decompressed = NULL;

	/* First pass (1): calculate decompressed size.
	 *                 (pass-1 is skipped if *decompressed_lenp is
	 *                  non-zero).
	 * Second pass (2): perform actual decompression.
	 */

	if (*decompressed_lenp != 0LLU)
		pass++;

	for (; pass <= 2 ; pass++) {
		z_stream strm = {};
		gz_header hdr;
		char buf[512];
		char *p;
		int len;
		int r;
		
		if ((r = inflateInit2(&strm, 15+32)) != Z_OK)
			goto fail;

		strm.next_in = (unsigned char*)compressed;
		strm.avail_in = compressed_len;

		if ((r = inflateGetHeader(&strm, &hdr)) != Z_OK) {
			inflateEnd(&strm);
			goto fail;
		}

		if (pass == 1) {
			/* Use dummy output buffer */
			p = buf;
			len = sizeof(buf);
		} else {
			/* Use real output buffer */
			p = decompressed;
			len = (int)*decompressed_lenp;
		}

		do {
			strm.next_out = (unsigned char *)p;
			strm.avail_out = len;

			r = inflate(&strm, Z_NO_FLUSH);
			switch  (r) {
			case Z_STREAM_ERROR:
			case Z_NEED_DICT:
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				inflateEnd(&strm);
				goto fail;
			}

			if (pass == 2) {
				/* Advance output pointer (in pass 2). */
				p += len - strm.avail_out;
				len -= len - strm.avail_out;
			}

		} while (strm.avail_out == 0 && r != Z_STREAM_END);


		if (pass == 1) {
			*decompressed_lenp = strm.total_out;
			if (!(decompressed = (char*)malloc((size_t)(*decompressed_lenp)+1))) {
				inflateEnd(&strm);
				return NULL;
			}
			/* For convenience of the caller we nul-terminate
			 * the buffer. If it happens to be a string there
			 * is no need for extra copies. */
			decompressed[*decompressed_lenp] = '\0';
		}

		inflateEnd(&strm);
	}

	return decompressed;

fail:
	if (decompressed)
		free(decompressed);
	return NULL;
}

