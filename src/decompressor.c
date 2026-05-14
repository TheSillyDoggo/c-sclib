#include "decompressor.h"
#include <stdlib.h>
#include <string.h>
#include <lzma.h>
#include <zstd.h>
#include <stdio.h>

static int decompress_lzma(const uint8_t* data, size_t size, void** outData, size_t* outSize)
{
    size_t allocSize = size * 20;

    uint8_t* output = malloc(allocSize);

    if (!output)
    {
        free(output);
        return SCLIB_DECOMPRESS_OUTOFMEM;
    }

    lzma_stream strm = LZMA_STREAM_INIT;

    lzma_ret ret = lzma_alone_decoder(&strm, UINT64_MAX);

    if (ret != LZMA_OK)
    {
        free(output);
        return 0;
    }

    strm.next_in = data;
    strm.avail_in = size;

    strm.next_out = output;
    strm.avail_out = allocSize;

    ret = lzma_code(&strm, LZMA_FINISH);

    if (!(ret == LZMA_STREAM_END || ret == LZMA_OK))
    {
        lzma_end(&strm);
        free(output);
        return -ret;
    }

    *outData = output;
    *outSize = strm.total_out;

    lzma_end(&strm);
    return 0;
}

static int decompress_zstd(const uint8_t* data, size_t size, void** outData, size_t* outSize)
{
    size_t decompressedSize = ZSTD_getFrameContentSize(data, size);

    if (decompressedSize == ZSTD_CONTENTSIZE_ERROR || decompressedSize == ZSTD_CONTENTSIZE_UNKNOWN)
        return 1;

    void* output = malloc((size_t)decompressedSize);

    if (!output)
        return SCLIB_DECOMPRESS_OUTOFMEM;

    size_t result = ZSTD_decompress(
        output,
        (size_t)decompressedSize,
        data,
        size
    );

    if (ZSTD_isError(result))
    {
        free(output);
        return -((int)ZSTD_getErrorCode(result));
    }

    *outData = output;
    *outSize = result;

    return 0;
}

static int32_t bytesToInt32LE(uint8_t* bytes)
{
    int32_t value = (int32_t)(
        (uint32_t)bytes[0] << 0  |
        (uint32_t)bytes[1] << 8  |
        (uint32_t)bytes[2] << 16 |
        (uint32_t)bytes[3] << 24
    );
    return value;
}

static int32_t bytesToInt32BE(uint8_t* bytes)
{
    int32_t value = (int32_t)(
        (uint32_t)bytes[0] << 24 |
        (uint32_t)bytes[1] << 16 |
        (uint32_t)bytes[2] << 8  |
        (uint32_t)bytes[3] << 0
    );
    return value;
}

int scDecompress(void* data, size_t size, void** returnData, size_t* returnSize)
{
    if (!data)
        return SCLIB_DECOMPRESS_NODATA;

    if (!*returnData)
        return SCLIB_DECOMPRESS_NORETURNPTR;

    enum scSignature sig = scGetSignature(data, size);

    switch (sig)
    {
        case scSigNONE:
            *returnData = data;
            *returnSize = size;
            return 0;

        case scSigLZMA:
        {
            size_t fixedSize = size + 4;

            uint8_t* fixed = malloc(fixedSize);

            if (!fixed)
                return SCLIB_DECOMPRESS_OUTOFMEM;

            memcpy(fixed, data, 5);
            memset(fixed + 5, 0xFF, 8);
            memcpy(fixed + 13, data + 9, size - 9);

            int ret = decompress_lzma((const uint8_t*)fixed, fixedSize, returnData, returnSize);
            free(fixed);
            return ret;
        }

        case scSigSC:
        {
            int32_t read = 0x2;
            int32_t fileVer = bytesToInt32BE(data + read);
            read += sizeof(int32_t);

            if (fileVer == 4)
            {
                fileVer = bytesToInt32BE(data + read);
                read += sizeof(int32_t);
            }

            if (fileVer == 0x05000000 || fileVer == 0x06000000)
            {
                if (fileVer == 0x06000000)
                    read += sizeof(int16_t);

                int32_t metadataOffset = bytesToInt32LE(data + read); read += sizeof(int32_t);
                read += metadataOffset;
            }
            else
            {
                int32_t hashLen = bytesToInt32BE(data + read); read += sizeof(int32_t);
                read += hashLen;
            }

            return scDecompress(data + read, size - read, returnData, returnSize);
        }
            
        case scSigSCLZ:
            return SCLIB_DECOMPRESS_NOTIMPLEMENTED;
            
        case scSigSIG:
            return scDecompress(data + 0x44, size - 0x44, returnData, returnSize);

        case scSigZSTD:
            return decompress_zstd((const uint8_t*)data, size, returnData, returnSize);

        default:
            return SCLIB_DECOMPRESS_UNKNOWNSIGNATURE;
    }
}