#ifndef SCLIB_DECOMPRESSOR_H__
#define SCLIB_DECOMPRESSOR_H__

#include "signatures.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SCLIB_DECOMPRESS_OK (0)
#define SCLIB_DECOMPRESS_NOTIMPLEMENTED (5)
#define SCLIB_DECOMPRESS_NORETURNPTR (6)
#define SCLIB_DECOMPRESS_NODATA (7)
#define SCLIB_DECOMPRESS_UNKNOWNSIGNATURE (8)
// malloc failed </3
#define SCLIB_DECOMPRESS_OUTOFMEM (9)

/*
 *  Decompresses a supercell file format into the return variables
 *  You are responsible for free ing returnData
 *  Returns: 0 if worked successfully, < 0 for decoder errors, > 0 for general errors
*/
int scDecompress(void* data, size_t size, void** returnData, size_t* returnSize);

#ifdef __cplusplus
}
#endif
#endif