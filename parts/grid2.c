#include "app.h"

BYTE grid32_top_asset;
BYTE grid32_mid_asset;
BYTE grid32_bottom_asset;
BYTE char_asset;

UBYTE grid2_preload(){
    loader_next();
    grid32_top_asset = asset_loadImage("data/grid32-top.planes", 320, 9, 5, MEMF_FAST);
    loader_next();
    grid32_mid_asset = asset_loadImage("data/grid32-mid.planes", 320, 20, 5, MEMF_FAST);
    loader_next();
    grid32_bottom_asset = asset_loadImage("data/grid32-bottom.planes", 320, 20, 5, MEMF_FAST);
    loader_next();
    char_asset = asset_loadFile("data/naphtachars.chunky", MEMF_FAST);
    return 1;
}

grid2_generate(){
    UWORD y = 4;
    UBYTE i;
    asset_moveToChip(grid32_top_asset);
    asset_moveToChip(grid32_mid_asset);
    asset_moveToChip(grid32_bottom_asset);
    
    screen_drawAsset2(grid32_top_asset, 0, y); 
    y += 9;
    for (i = 0; i < 11; i++){
        screen_drawAsset2(grid32_mid_asset, 0, y); 
        y += 20;
    }
    screen_drawAsset2(grid32_bottom_asset, 0, y);

    asset_free(grid32_top_asset);
    asset_free(grid32_mid_asset);
    asset_free(grid32_bottom_asset);

}

void grid2(void){
    UBYTE loop = 1;
    UBYTE screen[14 * 28 * 3];
    UWORD lastLine = 14*26;
    int i = 0;
    UBYTE r;
    UBYTE g;
    UBYTE b;
    UWORD x;
    UWORD y;

    UWORD index;
    UWORD index2;
    UWORD ybelow;
    UWORD ybelow2;
    UWORD xleft;
    UWORD xright;
    UBYTE direction = 1;
    UWORD color;
    UWORD frameCount = 0;
    int screenIndex = 0;
    UWORD modPositon = 0;
    UBYTE isStep = 0; 
    UBYTE fireActive = TRUE;
    UBYTE isTick = FALSE;
    UWORD *imagelist = getAssetData(char_asset);
    UBYTE sequence = 1;

    if (isMouseDown()) return;

    copper_reset();
    screen_clear();
    palette_setBlack(32);
    WaitTOF();
    
    copper_setGrid32();
    screen_disableDoubleBuffer();

    grid2_generate();

    for (i = 0; i < 1176; i++){
        screen[i] = 0;

        if (i > lastLine*3){
            screen[i] = random(16);
        }
    }

    while (loop){
        WaitTOF();

        //isTick = frameCount % 8 == 0;
        isTick = TRUE;

        if (fireActive && isTick){
            for(y = 0; y < 26; y++)
            for(x = 0; x < 14; x++){
                
                index2 = (y*14 + x);
                index = index2 * 3;

                ybelow = index + 14*3 + 3;
                ybelow2 = ybelow + 14*3;
                xleft = ybelow - 3;
                xright = ybelow + 3;

                screen[index] =
                ((screen[ybelow]
                + screen[xleft]
                + screen[xright]
                + screen[ybelow2]) >> 2);

                screen[index+1] =
                ((screen[ybelow+1]
                + screen[xleft+1]
                + screen[xright+1]
                + screen[index+1]) >> 2);

                screen[index+2] =
                ((screen[ybelow+2]
                + screen[xleft+2]
                + screen[xright+2]
                + screen[ybelow2+2]) >> 2);
            }

            for (i = lastLine; i < 378; i++){
                index = i * 3;
                screen[index]  = random(16);
                screen[index+1]  = random(16);
                screen[index+2]  = random(16);
            }
        }
        
        copper_setGrid2Colors(screen);

        frameCount++;

        if (sequence == 2){
            
            for (i = 0; i < 350; i++){
                color = imagelist[i + screenIndex];
                index = (i * 3);
                screen[index] = (color & 0xf00) >> 8;
                screen[index+1] = (color & 0x0f0) >> 4;
                screen[index+2] = (color & 0x00f);
            }

            screenIndex += 14*25;
            if (screenIndex > (15*350)){
                screenIndex = screenIndex=8*350;
            }
        }

        if (sequence == 1){
            if (isStep && modPositon%2 == 0){
                for (i = 0; i < 350; i++){
                    color = imagelist[i + screenIndex];
                    if (color){
                        index = (i * 3);
                        screen[index] = (color & 0xf00) >> 8;
                        screen[index+1] = (color & 0x0f0) >> 4;
                        screen[index+2] = (color & 0x00f);
                    }
                }
                screenIndex += 14*25;

                if (screenIndex > (8*350)){
                    sequence = 2;
                    fireActive = FALSE;
                }
            }
        }


        isStep = mod_isStep();
        if (isStep) modPositon++;

        if (modPositon>25){
            sequence = 3;
            fireActive = TRUE;
        }

        if (modPositon>33) loop = 0;
        if (isMouseDown())loop = 0;
    

        if (loop == 0){
            asset_free(char_asset);
        }
    }



}