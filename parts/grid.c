#include "app.h"

BYTE grid16_top_asset;
BYTE grid16_mid_asset;
BYTE cycle1_asset;
BYTE cycle2_asset;
BYTE cycle3_asset;
BYTE cycle5_asset;
BYTE cycle6_asset;
BYTE cycle7_asset;
BYTE cycle8_asset;
BYTE cycle9_asset;
BYTE amigaOverlay;
BYTE overlay_fire_asset;
BYTE cycle13_asset;
BYTE parrot_asset;
BYTE greeting_asset;
BYTE naphtaOverlay;

UWORD *imagelist;
UBYTE maxFrame;

UBYTE grid_preload(void){
    loader_next();
    grid16_top_asset = asset_loadImage("data/grid15-top.planes", 320, 10, 4, MEMF_ANY); 
    loader_next();
    grid16_mid_asset = asset_loadImage("data/grid15-mid.planes", 320, 16, 4, MEMF_ANY); 
    
    loader_next();
    cycle1_asset = asset_loadFile("data/cycle1.chunky", MEMF_FAST);  // red rays
    loader_next();
    cycle2_asset = asset_loadFile("data/cycle2.chunky", MEMF_FAST);  // blue rays
    loader_next();
    cycle3_asset = asset_loadFile("data/cycle3.chunky", MEMF_FAST);  // RGB plasma
    loader_next();
    cycle5_asset = asset_loadFile("data/cycle5.chunky", MEMF_FAST);  // purlple wobble
    loader_next();
    cycle6_asset = asset_loadFile("data/cycle6.chunky", MEMF_FAST); // slow motions
    loader_next();
    cycle7_asset = asset_loadFile("data/cycle7.chunky", MEMF_FAST); // spiral
    loader_next();
    cycle8_asset = asset_loadFile("data/cycle8.chunky", MEMF_FAST); // purple fire
    loader_next();
    cycle9_asset = asset_loadFile("data/cycle9.chunky", MEMF_FAST);  // green cloud
    loader_next();
    cycle13_asset = asset_loadFile("data/cycle13.chunky", MEMF_ANY); // red spiral
    loader_next();
    parrot_asset = asset_loadFile("data/parrot.chunky", MEMF_ANY); 
    loader_next();
    overlay_fire_asset = asset_loadImage("data/fire.planes", 320, 768, 1, MEMF_ANY); 
    loader_next();
    amigaOverlay = asset_loadImage("data/gridoverlays1.planes", 320, 256, 1, MEMF_ANY); 
    loader_next();
    greeting_asset = asset_loadImage("data/greetings.planes", 208, 256, 1, MEMF_ANY); 
    loader_next();
    naphtaOverlay = asset_loadImage("data/naphta-overlay.planes", 320, 256, 1, MEMF_ANY);
    return 1;
}

void grid_generate(void){
    UWORD y = 0;
    UWORD i = 0;
    asset_moveToChip(grid16_top_asset);
    asset_moveToChip(grid16_mid_asset);

    screen_drawAsset(grid16_top_asset, 0, 0, 320, 10, 0, 0);
    y+=10;

    for (i = 0; i < 15; i++){
        screen_drawAsset(grid16_mid_asset, 0, y, 320, 16, 0, 0);
        y+=16;
    }
    screen_drawAsset(grid16_top_asset, 0, 245, 320, 10, 0, 0);
}

void grid_setCycle(BYTE cycle){
    imagelist = getAssetData(cycle);
    maxFrame = (getAssetSize(cycle) / 480)-1;
}

BYTE getNaphtaOverlay(){
    return naphtaOverlay;
}

void grid(UBYTE part){
    int frameIndex = 0;
    int frameCounter = 0;
    UBYTE loop = 1;
    UWORD modPositon = 0;
    UWORD *overlayStart;
    WORD overlayPos;
    BYTE direction = 40;

    if (isMouseDown()) return;

    
    palette_setBlack(32);
    copper_reset();
    copper_setGrid16();
    screen_disableDoubleBuffer();
    screen_clear();
    grid_generate();

    if (part == 1){
        asset_moveToChip(amigaOverlay);
        asset_moveToChip(naphtaOverlay);
        grid_setCycle(cycle1_asset);
    }else if (part == 2){
        grid_setCycle(cycle5_asset);
        asset_moveToChip(overlay_fire_asset);
        overlayStart = getAssetData(overlay_fire_asset);
        overlayPos = 0;
        screen_overrideBitPlane(4, overlayStart);
    } else{
        asset_moveToChip(greeting_asset);
        bitplane_fillRect(4, 0, 0, 216, 256, 1);
        screen_drawAssetPlane(greeting_asset, 4, 8, 0, 16, 256, 0, 0, 0);
        grid_setCycle(cycle2_asset);
        
    }
    

    while (loop){
        frameCounter++;
        WaitTOF();
        copper_setGridScreen(imagelist + frameIndex*240);

        frameIndex++;
        if (frameIndex > maxFrame){
            frameIndex = 0;
        }

        if (mod_isStep()){
            modPositon++;

            if (part == 1){
                if (modPositon==4){
                    screen_overrideBitPlane(4, getAssetData(naphtaOverlay));
                }


                if (modPositon==8){
                    screen_overrideBitPlane(4, 0);
                    grid_setCycle(cycle2_asset);
                }

                if (modPositon==12){
                    screen_overrideBitPlane(4, getAssetData(amigaOverlay));
                }

                if (modPositon==16){
                    screen_overrideBitPlane(4, 0);
                    grid_setCycle(cycle3_asset);
                }

                if (modPositon==20){
                    grid_setCycle(cycle13_asset);
                }

                if (modPositon==24){
                    grid_setCycle(parrot_asset);
                }

                if (modPositon==28){
                    grid_setCycle(cycle8_asset);
                }

                if (modPositon==32){
                    grid_setCycle(cycle5_asset);
                }

                if (modPositon==40){
                    grid_setCycle(cycle6_asset);
                }

                if (modPositon==48){
                    grid_setCycle(cycle7_asset);
                }

                if (modPositon==56){
                    grid_setCycle(cycle9_asset);
                }

                if (modPositon>63){
                    loop = 0;
                }
            } else if (part == 2){
                if (modPositon==16){
                    grid_setCycle(cycle8_asset);
                }

                if (modPositon>31){
                    loop = 0;
                }
            } else{
                if (modPositon==4){
                    screen_drawAssetPlane(greeting_asset, 4, 8, 0, 208, 45, 0, 0, 0);
                }

                if (modPositon==8){
                    screen_drawAssetPlane(greeting_asset, 4, 8, 0, 208, 85, 0, 0, 0);
                }

                if (modPositon==12){
                    screen_drawAssetPlane(greeting_asset, 4, 8, 0, 208, 125, 0, 0, 0);
                }

                if (modPositon==16){
                    screen_drawAssetPlane(greeting_asset, 4, 8, 0, 208, 164, 0, 0, 0);
                }

                if (modPositon==20){
                    screen_drawAssetPlane(greeting_asset, 4, 8, 0, 136, 205, 0, 0, 0);
                }

                if (modPositon==24){
                    screen_drawAssetPlane(greeting_asset, 4, 8, 0, 208, 256, 0, 0, 0);
                }

                if (modPositon>31){
                    loop = 0;
                }
            }
        }

        if (part == 2){
            overlayPos += direction;
                if (overlayPos > 8000){ // 200 lines
                    overlayPos = 8000;
                    direction = -40;
                }else{
                    if (overlayPos < 0){
                        overlayPos = 0;
                        direction = 40;
                    }
                }
                screen_overrideBitPlane(4, overlayStart + overlayPos);
        }

    

        if (isMouseDown()){
            loop = 0;
        }

        if (loop == 0){
            copper_reset();
            screen_overrideBitPlane(4, 0);
            if (part == 1){
                asset_free(cycle1_asset);
                asset_free(cycle2_asset);
                asset_free(cycle6_asset);
                asset_free(cycle7_asset);
                asset_free(cycle13_asset);
            }
            if (part == 2){
                asset_free(overlay_fire_asset);
                asset_free(cycle3_asset);
                asset_free(cycle8_asset);
            }
            if (part == 3){
                asset_free(cycle9_asset);
                asset_free(grid16_top_asset);
                asset_free(grid16_mid_asset);
            }
        
        }
    }
}