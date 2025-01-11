#include <exec/memory.h>
#include "app.h"


void end(void){
    UBYTE loop = 1;
    static UWORD end_palette[] = {0X000,0X011,0X112,0X123,0X234,0X246,0X357,0X749,0X853,0X479,0X59b,0Xc72,0X9bc,0Xf91,0Xcca,0Xdee};
    BYTE end_asset;
    UWORD frameCount=0;
    UWORD y = 0;
    UWORD x = 112;
    UWORD offsetY = 242;
    UWORD delay = 0;
    UBYTE drawStep = 1; 
    UBYTE blend = 0;
    UWORD blend2 = 255;
    UWORD i;
    screen_disableDoubleBuffer();
    screen_clear();


    end_asset = asset_loadImage("data/end.planes", 112, 484, 4, MEMF_CHIP);
    //screen_drawAsset(end_asset,0,0,112,256,0,0);


    if (isMouseDown()) return;

    palette_setBlack(32);
    //palette_set(end_palette,16);
    screen_disableDoubleBuffer();
    copper_reset();


    while (loop){
        WaitTOF();
        frameCount++;

        if (blend<255){
            blend++;
            for (i=1;i<32;i++){
                palette_setColor(i,palette_blendColor(0x000,end_palette[i],blend));
            }
        }

        if (blend2<255){
            if (blend2>0) blend2--;
            for (i=1;i<32;i++){
                palette_setColor(i,palette_blendColor(0x000,end_palette[i],blend2));
            }
        }

        if (delay == 0 && y<242){
            screen_drawAsset(end_asset,x,y,112,drawStep,0,y+offsetY);
            y += drawStep;

            if (y>=242){
                offsetY = 0;
                y=0;
            }
        }

        if (delay>0){
            delay--;
            if (delay==0) y+= drawStep;
        }

        if (offsetY == 0){
            if (y==40){
                delay = 20;
                y=41;
            }

            if (y==75){
                delay = 20;
                y=76;
            }

            if (y==82){
                delay = 40;
                y=83;
            }

            if (y==130){
                delay = 20;
                screen_drawAsset(end_asset,x,y-2,112,drawStep+4,0,y-2);
                y=131;
            }

             if (y==170){
                delay = 20;
                y=171;
            }
        }
        
        if (frameCount==650){
            blend2 = 254;
        }

        if (frameCount>900){
            loop = 0;
        }

        if (isMouseDown()){
            loop = 0;
        }

        if (loop==0){
             for (i=1;i<20;i++){
                printf("\n");
            }
            printf("------------------------\n");
            printf("\n");
            printf("        NAPHTA\n");
            printf("\n");
            printf("------------------------\n");
            printf("\n");
            printf("      By Steffest\n");
            printf("\n");
            printf(" Released at RSYNC 2025\n");
            printf("  11 January - Leuven\n");
            printf("\n");
            printf("\n");
            printf("  Thanks for watching!\n");
            for (i=1;i<5;i++){
                printf("\n");
            }

        }
    }

}