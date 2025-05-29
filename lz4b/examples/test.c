#include "../lib/lz4hc.h"
#include "../lib/lz4.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_THREADS 4
#define BUF_SIZE (1024 * 1024)

int main(int argc, char *argv[]) {
    if (argc!= 3) {
        fprintf(stderr, "Usage: %s input_file output_file\n", argv[0]);
        return 1;
    }

    FILE *inFile = fopen(argv[1], "rb");
    if (inFile == NULL) {
        perror("Error opening input file");
        return 1;
    }

    FILE *outFile = fopen(argv[2], "wb");
    if (outFile == NULL) {
        perror("Error opening output file");
        fclose(inFile);
        return 1;
    }

    // Initialize the compression context
    LZ4F_compressionContext_t ctx;
    LZ4F_preferences_t prefs;
    LZ4F_preferencesInit(&prefs);
    prefs.autoFlush = 1;
    prefs.compressionLevel = 3; // Adjust the compression level as needed
    prefs.frameInfo.blockMode = LZ4F_blockLinked;
    prefs.frameInfo.blockSizeID = LZ4F_max64KB;
    prefs.autoFlush = 1;
    prefs.contentChecksumFlag = 1;
    prefs.parallelJobsCount = MAX_THREADS;

    LZ4F_errorCode_t err = LZ4F_createCompressionContext(&ctx, LZ4F_VERSION, &prefs);
    if (LZ4F_isError(err)) {
        fprintf(stderr, "Failed to create compression context: %s\n", LZ4F_getErrorName(err));
        fclose(inFile);
        fclose(outFile);
        return 1;
    }

    // Allocate buffers
    char *inBuf = malloc(BUF_SIZE);
    char *outBuf = malloc(BUF_SIZE);
    if (inBuf == NULL || outBuf == NULL) {
        perror("Error allocating memory");
        LZ4F_freeCompressionContext(ctx);
        fclose(inFile);
        fclose(outFile);
        return 1;
    }

    // Compress the file
    size_t inRead = 0, outWritten = 0;
    while ((inRead = fread(inBuf, 1, BUF_SIZE, inFile)) > 0) {
        LZ4F_compressFrameBound(ctx, inRead, outBuf, BUF_SIZE);
        size_t compressedSize = LZ4F_compressUpdate(ctx, outBuf, BUF_SIZE, inBuf, inRead, NULL);
        if (compressedSize == 0) {
            fprintf(stderr, "Compression failed\n");
            free(inBuf);
            free(outBuf);
            LZ4F_freeCompressionContext(ctx);
            fclose(inFile);
            fclose(outFile);
            return 1;
        }
        fwrite(outBuf, 1, compressedSize, outFile);
        outWritten += compressedSize;
    }

    // Finalize the compression
    LZ4F_compressFrameBound(ctx, 0, outBuf, BUF_SIZE);
    size_t finalSize = LZ4F_compressUpdate(ctx, outBuf, BUF_SIZE, NULL, 0, NULL);
    if (finalSize == 0) {
        fprintf(stderr, "Finalization failed\n");
        free(inBuf);
        free(outBuf);
        LZ4F_freeCompressionContext(ctx);
        fclose(inFile);
        fclose(outFile);
        return 1;
    }
    fwrite(outBuf, 1, finalSize, outFile);
    outWritten += finalSize;

    // Clean up
    free(inBuf);
    free(outBuf);
    LZ4F_freeCompressionContext(ctx);
    fclose(inFile);
    fclose(outFile);

    printf("Compressed %zu bytes to %zu bytes\n", inRead, outWritten);

    return 0;
}