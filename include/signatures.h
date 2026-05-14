#ifndef SCLIB_SIGNATURES_H__
#define SCLIB_SIGNATURES_H__

#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

enum scSignature
{
    scSigNONE = 0,
    scSigLZMA = 1,
    scSigSC = 2,
    scSigSCLZ = 3, // LZHAM
    scSigSIG = 4,
    scSigZSTD = 5,
};

/*
 *  Parses the signature from the data
*/
enum scSignature scGetSignature(void* data, size_t size);

#ifdef __cplusplus
}
#endif
#endif