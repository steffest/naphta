#include <exec/memory.h>
#include "app.h"

BYTE spotLightAsset;
BYTE dancerAsset[16];

static UWORD dancer_palette[] = {0X000,0X011,0X112,0X122,0X123,0X234,0X246,0X543,0X357,0X479,0X976,0X59b,0X9bc,0Xccb,0Xdee,0Xfff};
static UWORD dancer_palette_violet[] = {0X000,0X001,0X102,0X112,0X113,0X324,0X337,0X442,0X549,0X56a,0X975,0X77d,0Xaad,0Xccb,0Xeef,0Xfff};
static UWORD dancer_palette_purple[] = {0X000,0X101,0X200,0X212,0X312,0X523,0X354,0X725,0Xa46,0Xb5a,0X699,0Xd6b,0Xead,0Xccd,0Xfef,0Xfff};
static UWORD dancer_palette_red[] = {0X000,0X111,0X311,0X211,0X312,0X522,0X624,0X540,0X834,0X945,0X960,0Xd47,0Xe87,0Xcb6,0Xedc,0Xfff};
static UWORD dancer_palette_green[] = {0X000,0X010,0X111,0X121,0X122,0X242,0X254,0X751,0X374,0X4b5,0Xca3,0X6c9,0Xbdb,0Xee9,0Xefd,0Xfff};
static UWORD dancer_palette_yellow[] = {0X000,0X110,0X111,0X320,0X442,0X552,0Xa44,0X763,0Xb93,0Xbb5,0Xf99,0Xeca,0Xfcc,0Xffe,0Xfff,0X000};
static UWORD dancer_palette_orange[] = {0X000,0X210,0X111,0X421,0X421,0X632,0X942,0Xb47,0Xb54,0Xf75,0Xfa6,0Xf7d,0Xfcb,0Xfcf,0Xfef,0Xfff};
static UWORD dancer_palette_turqois[] = {0X000,0X010,0X012,0X021,0X023,0X033,0X351,0X056,0X067,0X494,0X0a9,0X1bc,0X3dc,0Xada,0Xbfe,0Xfff};

UBYTE dancer_preload(){
    loader_next();
    spotLightAsset = asset_loadImage("data/spotlight.planes", 112, 242, 4, MEMF_ANY);
    loader_next();
    dancerAsset[0] = asset_loadImage("data/low1.planes", 96, 81, 4, MEMF_ANY);
    loader_next();
    dancerAsset[1] = asset_loadImage("data/low2.planes", 80, 76, 4, MEMF_ANY);
    loader_next();
    dancerAsset[2] = asset_loadImage("data/low3.planes", 80, 72, 4, MEMF_ANY);
    loader_next();
    dancerAsset[3] = asset_loadImage("data/low4.planes", 96, 74, 4, MEMF_ANY);
    loader_next();
    dancerAsset[4] = asset_loadImage("data/low5.planes", 80, 77, 4, MEMF_ANY);
    loader_next();
    dancerAsset[5] = asset_loadImage("data/low6.planes", 96, 80, 4, MEMF_ANY);
    loader_next();
    dancerAsset[6] = asset_loadImage("data/low7.planes", 64, 83, 4, MEMF_ANY);
    loader_next();
    dancerAsset[7] = asset_loadImage("data/low8.planes", 64, 90, 4, MEMF_ANY);
    loader_next();
    dancerAsset[8] = asset_loadImage("data/high1.planes", 64, 119, 4, MEMF_ANY);
    loader_next();
    dancerAsset[9] = asset_loadImage("data/high2.planes", 80, 121, 4, MEMF_ANY);
    loader_next();
    dancerAsset[10] = asset_loadImage("data/high3.planes", 112, 116, 4, MEMF_ANY);
    loader_next();
    dancerAsset[11] = asset_loadImage("data/high4.planes", 64, 133, 4, MEMF_ANY);
    loader_next();
    dancerAsset[12] = asset_loadImage("data/high5.planes", 80, 144, 4, MEMF_ANY);
    loader_next();
    dancerAsset[13] = asset_loadImage("data/high6.planes", 112, 140, 4, MEMF_ANY);
    loader_next();
    dancerAsset[14] = asset_loadImage("data/high7.planes", 64, 150, 4, MEMF_ANY);
    loader_next();
    dancerAsset[15] = asset_loadImage("data/high8.planes", 48, 132, 4, MEMF_ANY);
    
    return 1;
}

void drawDancer(BYTE index, UBYTE bottom, UBYTE series){
    UWORD x = index*32;
    UWORD x2;
    UWORD dancer = dancerAsset[index + series];
    struct ASSET asset = *getAsset(dancer);
    if (x>208) x=208;
    x2 = x;
    if (asset.width < 96) x2 += 16;
    if (index+series == 9) x2 -= 16;
    if (index+series == 12) x2 -= 16;
    if (index+series == 15) x2 += 16;
    screen_clear();
    screen_drawAsset(spotLightAsset, x, 0, 112, 242, 0, 0);
    screen_drawAsset(dancer, x2, 242-asset.height-bottom, asset.width, asset.height, 0, 0);
}

void dancer(void){
    UBYTE loop = 1;
    UBYTE i;
    UBYTE imageIndex = 0;
    UBYTE colorIndex = 0;
    UBYTE offset = 0;
    UWORD modPositon = 0;
    UBYTE isStep = 1;

    if (isMouseDown()) return;
    
    screen_clear();
    copper_reset();
    WaitTOF();
    palette_set(dancer_palette,16);
    asset_moveToChip(spotLightAsset);
    for (i = 0; i < 16; i++){
        asset_moveToChip(dancerAsset[i]);
    }
    //asset_moveToChip(naphtaOverlay);
    screen_disableDoubleBuffer();


    //drawDancer(7,1,8);

    while (loop){
        WaitTOF();

        if (isStep){
            if (colorIndex == 0) palette_set(dancer_palette,16);
            if (colorIndex == 1) palette_set(dancer_palette_violet,16);
            if (colorIndex == 2) palette_set(dancer_palette_purple,16);
            if (colorIndex == 3) palette_set(dancer_palette_red,16);
            if (colorIndex == 4) palette_set(dancer_palette_orange,16);
            if (colorIndex == 5) palette_set(dancer_palette_yellow,16);
            if (colorIndex == 6) palette_set(dancer_palette_green,16);
            if (colorIndex == 7) palette_set(dancer_palette_turqois,16);

            drawDancer(imageIndex, 0, offset);

            imageIndex++;
            if (imageIndex > 7){
                imageIndex = 0;
                offset = 8-offset;
            }
            if (modPositon > 14){
                colorIndex++;
                if (colorIndex > 7) colorIndex = 0;
            }
            if (modPositon == 32){
                asset_moveToChip(getNaphtaOverlay());
                screen_overrideBitPlane(4, getAssetData(getNaphtaOverlay()));
            }
   
        }


        isStep = mod_isStep();
        if (isStep) modPositon++;

        if (modPositon>63){
            loop = 0;
        }
    
        if (isMouseDown()){
            loop = 0;
        }

        if (loop == 0){
            for (i = 0; i < 16; i++) {
                asset_free(dancerAsset[i]);
            }
            screen_overrideBitPlane(4, 0);
            //asset_free(spotLightAsset);
        }
    }


   
    

}