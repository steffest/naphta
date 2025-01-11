#include "app.h"
//#include "src/shrinkler/ShrinklerDecompress.h"

#include <stdio.h>
#include <exec/memory.h>
#include <exec/types.h>
#include <proto/graphics.h>
#include <intuition/intuition.h>

static struct ASSET assets[50];
char assetCount = -1;

BYTE asset_loadFile(char *filename, UBYTE memoryType){
    // Load a file from the filesystem into

    int i;
    UBYTE *memory;
    FILE *fileHandle;
    ULONG fileSize;

    int MEMTYPE = MEMF_ANY;
    if (memoryType==MEMF_CHIP) MEMTYPE = MEMF_CHIP;
    if (memoryType==MEMF_FAST) MEMTYPE = MEMF_FAST;
 
    assetCount++;
    assets[assetCount].type = 1;
    assets[assetCount].memoryType = memoryType;

    if (WRITE_DEBUG) printf("Asset: loading file %s\n",filename);
  
    fileHandle = fopen(filename, "rb");
    if (fileHandle == NULL) {
        printf("Asset Failed to open file %s\n",filename);
        return -1;
    }

    // Get the file size
    fseek(fileHandle,0,2);
    fileSize=ftell(fileHandle);
    if (WRITE_DEBUG) printf("file is %d bytes\n",fileSize);
    rewind(fileHandle);
   
    // Allocate cmemory
    memory = AllocMem(fileSize, MEMTYPE);
    if (memory == NULL) {
        printf("Asset: Failed to allocate memory for %s\n",filename);
        fclose(fileHandle);
        assets[assetCount].type = 0;
        return -1;
    }

    // Read the file into memory
    if (fread(memory, sizeof(UBYTE), fileSize, fileHandle) != fileSize) {
        printf("Failed to read file\n");
        FreeMem(memory, fileSize);
        assets[assetCount].type = 0;
        fclose(fileHandle);
        return -1;
    }
    fclose(fileHandle);
 
    if (memoryType == MEMF_ANY){
        if (system_isChipMem(memory)){
            assets[assetCount].memoryType = MEMF_CHIP;
            if (WRITE_DEBUG) printf("File landed in CHIP memory\n");
        }else{
            if (WRITE_DEBUG) printf("File landed in FAST memory\n");
        }
    }

    assets[assetCount].data = memory;
    assets[assetCount].size = fileSize;

    return assetCount;

}

// Shrinkler compressed files, with header
/*BYTE asset_loadCompressed(char *filename, UBYTE memoryType, UBYTE andDecompress){
    if (andDecompress){
        UBYTE *memory;
        
        int MEMTYPE = MEMF_ANY;
        if (memoryType==MEMF_CHIP) MEMTYPE = MEMF_CHIP;
        if (memoryType==MEMF_FAST) MEMTYPE = MEMF_FAST;

        assetCount++;
        assets[assetCount].type = 1;
        assets[assetCount].memoryType = memoryType;

        memory = shrinklerLoad(filename, memoryType, 0, 0, 0);
        assets[assetCount].data = memory;
        //assets[assetCount].size = 411678;
        assets[assetCount].size = 40960; // TODO: how to get the size of the decompressed file?

        return assetCount;
    }else{
        return asset_loadFile(filename, memoryType);
    }
    
}*/

BYTE asset_loadImage(char *filename, UWORD width, UWORD height, UBYTE bitplanes, UBYTE memoryType){
    BYTE index = asset_loadFile(filename, memoryType);
    if (index<0) return -1;
    assets[index].width = width;
    assets[index].height = height;
    assets[index].planeCount = bitplanes;
    assets[index].planeSize = (width >> 3) * height;
    return index;
}

/*BYTE asset_loadImageCompressed(char *filename, UWORD width, UWORD height, UBYTE bitplanes, UBYTE memoryType, UBYTE andDecompress){
    char index = asset_loadCompressed(filename, memoryType,andDecompress);
    if (index<0) return -1;
    assets[index].width = width;
    assets[index].height = height;
    assets[index].planeCount = bitplanes;
    assets[index].planeSize = (width >> 3) * height;
    return index;
}*/


UBYTE *getAssetData(BYTE index){
    return assets[index].data;
}

UWORD getAssetSize(BYTE index){
    return assets[index].size;
}

BOOL asset_moveToChip(BYTE index){
    UBYTE *chipMemory;
    if (assets[index].memoryType==MEMF_CHIP) return TRUE;
    chipMemory = AllocMem(assets[index].size, MEMF_CHIP);
    if (chipMemory == NULL) {
        printf("Failed to allocate chip memory while moving asset %d\n",index);
        return FALSE;
    }
    CopyMem(assets[index].data, chipMemory, assets[index].size);
    FreeMem(assets[index].data, assets[index].size);
    assets[index].data = chipMemory;
    assets[index].memoryType = MEMF_CHIP;
    return TRUE;
}

struct ASSET *getAsset(BYTE index){
    return &assets[index];
}

void asset_free(BYTE index){
    if (assets[index].type>0){
        int i;
        if (WRITE_DEBUG) printf("freeing asset %d of type %d\n",index,assets[index].type);

        if (assets[index].type==1){
            FreeMem(assets[index].data, assets[index].size);
        }
        assets[index].type = 0;
    }
}

void free_assets(void){
    int i;
    if (WRITE_DEBUG) printf("Asset: freeing %d\n",assetCount+1);
    for (i=0; i<=assetCount; i++){
        asset_free(i);
    }
}