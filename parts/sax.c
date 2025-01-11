#include <exec/memory.h>
#include "app.h"


static UWORD sax_palette[] = {0X000,0X200,0X023,0X311,0X600,0X621,0X045,0X533,0Xb11,0Xa43,0X07c,0Xf11,0Xf52,0Xc76,0Xf93,0Xdcb,0X000,0X200,0X012,0X311,0X005,0X129,0X045,0X334,0X329,0X45b,0X07c,0X43d,0X45f,0X47b,0X79f,0X9db};
BYTE sax_asset;

UBYTE sax_preload(){
    loader_next();
    sax_asset = asset_loadImage("data/sax.planes", 320, 240, 5, MEMF_ANY);
    return 1;
}

void sax(void){
    UWORD frameCount = 0;
    int i;
    UBYTE loop = 1;
    UBYTE paletteStart = 0;
    UWORD burstIndex = 0;
    UWORD burstColor = 0;
    UWORD burstCo[16];
    UWORD burstCo2[16];
    UWORD burstSteps[16];
    UBYTE burstColors[16];
    UWORD burstTimer = 0;
    UWORD masterFade = 0;
    UWORD color;
    UWORD color2;
    UWORD index;
    UWORD modPositon = 0;
    UBYTE isStep = 0;
    WORD blend = 0;

    if (isMouseDown()) return;

    palette_setBlack(32);
    //palette_set(sax_palette_red,16);
    asset_moveToChip(sax_asset);
    screen_disableDoubleBuffer();
    copper_reset();

    copper_start();
    
    for (i = 31; i >= 16; i--){
        copper_waitForLine(160 - i);
        copper_setColor(i, 0);
    }
    for (i = 16; i < 32; i++){
        copper_waitForLine(150 + i);
        copper_setColor(i, 0);
    }
    copper_end();

    WaitTOF();
    screen_drawAsset(sax_asset, 0, 0, 320, 240, 0, 0);

    for (i = 0; i < 16; i++){
        burstSteps[i] = 0;
        burstColors[i] = 0;
        burstCo[15 - i] = 3 + (i * 4);
        burstCo2[i] = 64 + (i * 4) + 3;
    }



    while (loop){
        WaitTOF();
        frameCount++;
        burstTimer++;



        if (isStep){

            if (modPositon%2 == 0 && modPositon<29){
                burstColor = random(90) + 16;
                burstIndex = 1;
                burstTimer = 0;
            }

            paletteStart = 16-paletteStart;
            if (masterFade<255){
                for (i = 0; i < 16; i++){
                    palette_setColor(i, palette_blendColor(0x000,sax_palette[paletteStart+i], masterFade));
                }  
                masterFade+=8;
                 if (masterFade>255) masterFade = 255;
             } else{
                for (i = 0; i < 16; i++){
                    palette_setColor(i, sax_palette[paletteStart+i]);
                }   
            }
        }

        if (burstIndex>0){
            burstSteps[burstIndex-1] = 15;     
            burstColors[burstIndex-1] = burstColor;

            burstIndex++;
            if (burstIndex>16){
                burstIndex = 0;
            }  
        }

        if (burstTimer == 18){
            burstColor = random(90) + 16;
            burstIndex = 1;
        }
        if (burstTimer == 36){
            burstColor = random(90) + 16;
            burstIndex = 1;
        }

        for (i = 0 ; i < 16; i++){
            if (burstSteps[i]>0){

                blend = (burstSteps[i]<<4) - (i<<2);
                if (blend < 0) blend = 0;

                color = palette_blendColor(0x000, palette_getBrightColor(burstColors[i]-i), blend);
                color2 = palette_blendColor(color, 0x000, 92);
            
                palette_setColor(i+16, color2);            
                copper_setExtention(burstCo[i],color);
                copper_setExtention(burstCo2[i],color2);
                burstSteps[i]--;
                
            }
        }

        isStep = mod_isStep();
        if (isStep) modPositon++;
        
        if (modPositon>31){
            loop = 0;
        }

        if (isMouseDown()){
            loop = 0;
        }

        if (loop == 0){
           asset_free(sax_asset);
        }
    }

}