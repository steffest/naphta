#include <exec/types.h>

struct ASSET {
    UBYTE type;
    UBYTE memoryType;
    UBYTE planeCount;
    UWORD planeSize;
    UWORD width;
    UWORD height;
    UBYTE *data;
    UWORD size;
};


BYTE asset_loadFile(char *filename, UBYTE memoryType);
BYTE asset_loadCompressed(char *filename, UBYTE memoryType, UBYTE andDecompress);
BYTE asset_loadImage(char *filename, UWORD width, UWORD height, UBYTE bitplanes, UBYTE memoryType);
BYTE asset_loadImageCompressed(char *filename, UWORD width, UWORD height, UBYTE bitplanes, UBYTE memoryType, UBYTE andDecompress);
BOOL asset_decompress(BYTE index);
UBYTE *getAssetData(BYTE index);
UWORD getAssetSize(BYTE index);
BOOL asset_moveToChip(BYTE index);
struct ASSET *getAsset(BYTE index);
void free_assets(void);