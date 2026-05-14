#include "signatures.h"
#include <string.h>

#define MAGIC_SC   "SC"
#define MAGIC_SCLZ "SCLZ"
#define MAGIC_SIG  "Sig:"

static const unsigned char MAGIC_ZSTD[] = {
    0x28, 0xB5, 0x2F, 0xFD
};

enum scSignature scGetSignature(void* _data, size_t size)
{
    if (!_data)
        return scSigNONE;

    unsigned char* data = (unsigned char *)_data;

    if (size >= 5)
    {
        // 0x3 is maybe 0x0
        // if re.match(b"\x00\x00?\x00", buffer[1:5]):
        if (data[0x1] == 0x0 && data[0x2] == 0x0 && data[0x4] == 0x0)
            return scSigLZMA;
    }

    if (size >= 4 && memcmp(data, MAGIC_ZSTD, 4) == 0)
        return scSigZSTD;

    if (size >= 4 && memcmp(data, MAGIC_SCLZ, 4) == 0)
        return scSigSCLZ;

    if (size >= 2 && memcmp(data, MAGIC_SC, 2) == 0)
        return scSigSC;

    if (size >= 4 && memcmp(data, MAGIC_SIG, 4) == 0)
        return scSigSIG;

    return scSigNONE;
}