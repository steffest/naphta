#ifndef SHRINKLER_DECOMPRESS_H
#define SHRINKLER_DECOMPRESS_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

#ifndef SDI_COMPILER_H
#include <SDI_compiler.h>
#endif

struct ShrinklerHeader {
    UBYTE id[4]; // "Shri"
    UBYTE versionMajor, versionMinor;
    USHORT headerSize;
    UBYTE compSize1;
    UBYTE compSize2;
    UBYTE compSize3;
    UBYTE compSize4;
    ULONG unCompSize;
    ULONG buffer1;
    ULONG flags;
};

struct ShrinklerFile {
    struct ShrinklerHeader header;
    UBYTE *compressedData;
};

void ASM shrinklerDecompress(
    REG(a0, void *source),
    REG(a1, void *destination),
    REG(a2, void *callback),
    REG(a3, void *param),
    REG(d2, UBYTE param2),
    REG(d7, UBYTE parity)
    );
int ASM shrinklerLoad(
    REG(a0, void *name),
    REG(d4, UBYTE memoryType),
    REG(d5, UBYTE alignment),
    REG(a2, void *callback),
    REG(a3, void *param)
    );
void ASM shrinklerFree(REG(a0, void *loadedData));

#endif 