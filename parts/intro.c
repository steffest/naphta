#include "app.h"

BYTE logo_asset;
BYTE fog_asset;

static UWORD intro_palette2[] =  {0X000,0X122,0X233,0X345,0X467,0X578,0X799,0X8bb,0Xdca,0X820,0X963,0Xe61,0Xc95,0Xfb3,0Xfe6,0Xffd};
static UWORD intro_palette[] =  {0X000,0X122,0X233,0X345,0X467,0X578,0X799,0X8bb,0X233,0X011,0X000,0X122,0X133,0X233,0X233,0X233};

static UBYTE palettergb[32*3];

UBYTE intro_preload(){
    loader_next();
    logo_asset = asset_loadImage("data/naphta.planes", 320, 256, 4, MEMF_CHIP);
    loader_next();
    fog_asset = asset_loadImage("data/fog1.planes", 320, 768, 1, MEMF_FAST);
    return 1;
}

BYTE getFogAsset(void){
    return fog_asset;
}

void setFade(amount,fromIndex,toIndex){
    UBYTE i;
    UBYTE index = fromIndex * 3;
    for (i = fromIndex; i < toIndex; i++) {
        if (amount<255){
            palette_setColor(i, palette_blendRGB(0,0,0,palettergb[index],palettergb[index+1],palettergb[index+2],amount));
        }else{
            palette_setColor(i, intro_palette[i]);
        }
       index+=3;
    }
}

void intro(){
    UBYTE loop = 1;
    WORD fogY = 0;
    UBYTE fogActive = FALSE;
    int frameCount = 0;
    struct ASSET *fog;
    UBYTE *tempPlane;
    WORD masterFade = 0;
    int i;
    int index = 0;
    UBYTE fadeOut = 0;
    UWORD modTrigger = 0;

    UWORD palette_overlay_red1[8];
    UWORD palette_overlay_red2[8];
    UWORD palette_overlay_red3[8];
    UWORD palette_overlay_red4[8];


    UWORD palette_overlay_gold1[8];
    UWORD palette_overlay_gold2[8];
    UWORD palette_overlay_gold3[8];
    UWORD palette_overlay_gold4[8];


    UWORD rowY1 = 0;
    UWORD rowY2 = 0;
    UWORD rowY3 = 0;
    UWORD rowY4 = 0;
    UWORD rowY5 = 0;


    // generate transition palettes
    for (i = 0; i < 8; i++){
        palette_overlay_red1[i] = palette_blendColor(intro_palette[i+8], 0xf00, 32);
        palette_overlay_red2[i] = palette_blendColor(intro_palette[i+8], 0xf00, 64);
        palette_overlay_red3[i] = palette_blendColor(intro_palette[i+8], 0xf00, 96);
        palette_overlay_red4[i] = palette_blendColor(intro_palette[i+8], 0xf00, 128);

        palette_overlay_gold1[i] = palette_blendColor(intro_palette[i+8], intro_palette2[i+8], 32);
        palette_overlay_gold2[i] = palette_blendColor(intro_palette[i+8], intro_palette2[i+8], 64);
        palette_overlay_gold3[i] = palette_blendColor(intro_palette[i+8], intro_palette2[i+8], 128);
        palette_overlay_gold4[i] = palette_blendColor(intro_palette[i+8], intro_palette2[i+8], 192);
    }

    palette_setBlack(16);
    for (i = 16; i < 32; i++){
        palette_setColor(i, intro_palette[i-16]);
    }

    // generate cropped fog overlay
    fog = getAsset(fog_asset);
    tempPlane = AllocMem(fog->size, MEMF_CHIP | MEMF_CLEAR);

    if (tempPlane == NULL){
        printf("Failed to allocate memory for fog overlay\n");
        return;
    }
    CopyMem(fog->data, tempPlane, fog->size);
    blitter_fillRect(tempPlane, 0, 0, 58, 768, 0);
    blitter_fillRect(tempPlane, 268, 0, 52, 768, 0);

    // generate RGB values
    for (i = 0; i < 16; i++){
        palettergb[i*3] = (intro_palette[i] & 0xf00) >> 8;
        palettergb[i*3+1] = (intro_palette[i] & 0x0f0) >> 4;
        palettergb[i*3+2] = (intro_palette[i] & 0x00f);
    }


    WaitTOF();
    screen_drawAsset(logo_asset, 0, 0, 320, 256, 0, 0);
    //screen_overrideBitPlane(4, (APTR) tempPlane);
    screen_copyToBuffer();
    
    while (loop){
        WaitTOF();
        frameCount++;

        if (frameCount < 66){
            // fade in
            masterFade += 4;
            if (masterFade > 255) masterFade = 255;
            setFade(masterFade,1,8);
            if (frameCount == 65) masterFade = 0;
        } else if (frameCount < 100){
            // fade in 2
            masterFade += 8;
            if (masterFade > 255) masterFade = 255;
            setFade(masterFade,8,16);
            if (frameCount == 99){
                fogActive = TRUE;
            }
        } else if (frameCount == 100){
            rowY1 = 230;
            rowY2 = 231;
            rowY3 = 232;
            rowY4 = 233;
            copper_start();

            copper_waitForLine(rowY1);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red1[i-24]);
            }

            copper_waitForLine(rowY2);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red2[i-24]);
            }

            copper_waitForLine(rowY3);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red3[i-24]);
            }

            copper_waitForLine(rowY4);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red4[i-24]);
            }
            copper_end();
        } else if (frameCount < 260){
            if (rowY1 > 20) rowY1--;
            if (frameCount>120 && rowY2 > 21) rowY2--;
            if (frameCount>140 && rowY3 > 23) rowY3--;
            if (frameCount>160 && rowY4 > rowY3) rowY4--;
            
            copper_setExtention(0, rowY1*256 + 7);
            copper_setExtention(18, rowY2*256 + 7);
            copper_setExtention(36, rowY3*256 + 7);
            copper_setExtention(54, rowY4*256 + 7);
        } else if (frameCount == 260){
            for (i = 24; i < 32; i++) {
                palette_setColor(i, palette_overlay_red4[i-24]);
            }
            copper_reset();
        } else if (frameCount == 261){
            for (i = 0; i < 8; i++){
                palette_overlay_red1[i] = palette_blendColor(intro_palette[i], 0xfaa, 32);
                palette_overlay_red2[i] = palette_blendColor(intro_palette[i], 0xf88, 64);
                palette_overlay_red3[i] = palette_blendColor(intro_palette[i], 0xf44, 96);
                palette_overlay_red4[i] = palette_blendColor(intro_palette[i], 0xf22, 112);
            }
        } else if (frameCount == 263){
            // init next copperlist with "invisible" colors to avoid flickering
            rowY1 = 177;
            rowY2 = 178;
            rowY3 = 179;
            copper_start();
            copper_waitForLine(rowY1);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(rowY2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(rowY3);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(180);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(181);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(182);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(183);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            copper_waitForLine(184);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }

            copper_waitForLine(220);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold1[i-8]);
            }
            copper_waitForLine(221);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold2[i-8]);
            }
            copper_waitForLine(222);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold3[i-8]);
            }
            copper_waitForLine(223);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold4[i-8]);
            }
            copper_waitForLine(224);
            for (i = 8; i < 16; i++){
                copper_setColor(i,intro_palette2[i]);
            }

            copper_waitForLine(225);
            for (i = 24; i < 32; i++){
                copper_setColor(i,intro_palette[i-24]);
            }
            copper_waitForLine(226);
            for (i = 24; i < 32; i++){
                copper_setColor(i,intro_palette[i-24]);
            }
            copper_waitForLine(227);
            for (i = 24; i < 32; i++){
                copper_setColor(i,intro_palette[i-24]);
            }

            copper_end();
        }else if (frameCount == 265){
            for (i = 0; i < 8; i++){
                copper_setExtention(3 + (i<<1),palette_overlay_red1[i]);
                copper_setExtention(21 + (i<<1),palette_overlay_red2[i]);
                copper_setExtention(39 + (i<<1),palette_overlay_red3[i]);
                copper_setExtention(57 + (i<<1),palette_overlay_red4[i]);
                copper_setExtention(75 + (i<<1),palette_overlay_red3[i]);
                copper_setExtention(93 + (i<<1),palette_overlay_red2[i]);
                copper_setExtention(111 + (i<<1),palette_overlay_red1[i]);
            }
        }else if (frameCount <370){
            if (rowY1 > 50) rowY1--;
            if (frameCount>275 && rowY2 > 60) rowY2--;
            if (frameCount>285 && rowY3 > 80) rowY3--;

            copper_setExtention(0, rowY1*256 + 7);
            copper_setExtention(18, rowY2*256 + 7);
            copper_setExtention(36, rowY3*256 + 7);
        } else if (frameCount == 370){
            rowY1 = 220;
            rowY2 = 221;
            rowY3 = 222;
            rowY4 = 223;
            rowY5 = 224;
        } else if (frameCount < 440){
            if (rowY1 > 185) rowY1--;
            if (frameCount>400 && rowY2>190) rowY2--;
            if (frameCount>420 && rowY3>200) rowY3--;
            if (frameCount>440 && rowY4>210) rowY4--;
            if (frameCount>450 && rowY5>220) rowY5--;

            copper_setExtention((18*8), rowY1*256 + 7);
            copper_setExtention((18*9), rowY2*256 + 7);
            copper_setExtention((18*10), rowY3*256 + 7);
            copper_setExtention((18*11), rowY4*256 + 7);
            copper_setExtention((18*12), rowY5*256 + 7);

            copper_setExtention((18*3), (rowY1-40)*256 + 7);
            

        } else if (frameCount < 490){
            if (rowY1 > 145){
                rowY1--;
                if (frameCount%2==0) rowY2--;
                if (frameCount%4==0) rowY3--;
                if (frameCount%5==0) rowY4--;
                if (frameCount%6==0) rowY5--;
            }
            copper_setExtention((18*3), (rowY1-40)*256 + 7);
            copper_setExtention((18*4), (rowY1-10)*256 + 7);
            copper_setExtention((18*5), (rowY1-5)*256 + 7);
            copper_setExtention((18*6), (rowY1-3)*256 + 7);
            copper_setExtention((18*7), (rowY1-1)*256 + 7);
        

            copper_setExtention((18*8), rowY1*256 + 7);
            copper_setExtention((18*9), rowY2*256 + 7);
            copper_setExtention((18*10), rowY3*256 + 7);
            copper_setExtention((18*11), rowY4*256 + 7);
            copper_setExtention((18*12), rowY5*256 + 7);
        } else if (frameCount < 570){
            if (frameCount%2==0){
                if (rowY1 > 125) rowY1--;
                if (rowY2 > 130) rowY2--;
                if (rowY3 > 131) rowY3--;
                if (rowY4 > 132) rowY4--;
                if (rowY5 > 133) rowY5--;

                copper_setExtention((18*4), (rowY1-15)*256 + 7); // 110 // red3
                
                copper_setExtention((18*5), (rowY1-5)*256 + 7); // red2
                copper_setExtention((18*6), (rowY1-3)*256 + 7); // red1
                copper_setExtention((18*7), (rowY1-1)*256 + 7); // final 124
        
                copper_setExtention((18*8), rowY1*256 + 7); // gold 1
                copper_setExtention((18*9), rowY2*256 + 7);
                copper_setExtention((18*10), rowY3*256 + 7);
                copper_setExtention((18*11), rowY4*256 + 7);
                copper_setExtention((18*12), rowY5*256 + 7); // final
            }
        } else if (frameCount == 570){
            // rowY1: 125
            // rowY2: 130
            // rowY3: 150
            // rowY4: 162
            // rowY5: 174

            copper_setExtentionIndex(18*5 + 2);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold1[i-8]);
            }

            copper_setExtentionIndex(18*6 + 2); // 122
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red2[i-16]);
            }
            copper_setExtentionIndex(18*7 + 2); // 124
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red1[i-16]);
            }
            copper_setExtentionIndex(18*8 + 2); // 125
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }

            rowY1 -= 5;

        } else if (frameCount < 580){
                if (rowY1>111) rowY1--;
                if (rowY2>126) rowY2--;
                rowY3--;
                rowY4--;
                rowY5--;
                copper_setExtention((18*5), rowY1*256 + 7); // gold 1
                copper_setExtention((18*9), rowY2*256 + 7);
                copper_setExtention((18*10), rowY3*256 + 7);
                copper_setExtention((18*11), rowY4*256 + 7);
                copper_setExtention((18*12), rowY5*256 + 7); // final
        } else if (frameCount == 580){
            // rowY1: 111
            // rowY2: 126
            // rowY3: 141
            // rowY4: 153
            // rowY5: 165

            copper_setExtentionIndex(18*6 + 2);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold2[i-8]);
            }

            copper_setExtentionIndex(18*7 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red2[i-16]);
            }
            copper_setExtentionIndex(18*8 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red1[i-16]);
            }
            copper_setExtentionIndex(18*9 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }

            rowY2 = 122;
        } else if (frameCount < 595){
                if (rowY2>112) rowY2--;
                rowY3--;
                rowY4--;
                rowY5--;
                copper_setExtention((18*6), rowY2*256 + 7);
                copper_setExtention((18*10), rowY3*256 + 7);
                copper_setExtention((18*11), rowY4*256 + 7);
                copper_setExtention((18*12), rowY5*256 + 7); // final
        } else if (frameCount == 595){
            //printf("rows: %d %d %d %d %d\n", rowY1, rowY2, rowY3, rowY4, rowY5);
            
           
            // rowY1: 111
            // rowY2: 112
            // rowY3: 127
            // rowY4: 139
            // rowY5: 151

            copper_setExtentionIndex(18*7 + 2);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold3[i-8]);
            }

            copper_setExtentionIndex(18*8 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red2[i-16]);
            }
            copper_setExtentionIndex(18*9 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red1[i-16]);
            }
            copper_setExtentionIndex(18*10 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }

            rowY3 = 122;

        } else if (frameCount < 607){
                if (rowY3>113) rowY3--;
                rowY4--;
                rowY5--;
                copper_setExtention((18*7), rowY3*256 + 7);
                copper_setExtention((18*11), rowY4*256 + 7);
                copper_setExtention((18*12), rowY5*256 + 7); // final
        } else if (frameCount == 607){
               // rowY3: 113
               // rowY4: 128
               // rowY5: 140


            copper_setExtentionIndex(18*8 + 2);
            for (i = 8; i < 16; i++){
                copper_setColor(i,palette_overlay_gold4[i-8]);
            }

            copper_setExtentionIndex(18*9 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red2[i-16]);
            }
            copper_setExtentionIndex(18*10 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red1[i-16]);
            }
            copper_setExtentionIndex(18*11 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }

            rowY4 = 122;
                
        } else if (frameCount < 617){
                if (rowY4>114) rowY4--;
                rowY5--;
                copper_setExtention((18*8), rowY4*256 + 7);
                copper_setExtention((18*12), rowY5*256 + 7); // final
        } else if (frameCount == 617){
            copper_setExtentionIndex(18*9 + 2);
            for (i = 8; i < 16; i++){
                copper_setColor(i,intro_palette2[i]);
            }

            copper_setExtentionIndex(18*10 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red2[i-16]);
            }
            copper_setExtentionIndex(18*11 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,palette_overlay_red1[i-16]);
            }
            copper_setExtentionIndex(18*12 + 2);
            for (i = 16; i < 24; i++){
                copper_setColor(i,intro_palette[i-16]);
            }
            rowY1 = 125;
            rowY2 = 126;
            rowY3 = 127;
            rowY4 = 90;
        } else if (frameCount < 640){
            rowY1++;
            rowY2 += 1;
            rowY3 += 1;
            rowY4++;

            copper_setExtention((18*3), rowY4*256 + 7);
    
            copper_setExtention((18*10), rowY1*256 + 7);
            copper_setExtention((18*11), rowY2*256 + 7);
            copper_setExtention((18*12), rowY3*256 + 7);
        } else if (frameCount == 640){

            for (i = 0; i < 8; i++){
                palette_overlay_red1[i] = palette_blendColor(intro_palette[i+8], 0xf11, 128);
                palette_overlay_red4[i] = palette_blendColor(intro_palette2[i+8], 0xfaa, 128);

                palette_overlay_red3[i] = palette_blendColor(palette_overlay_red1[i], palette_overlay_red4[i], 64);
                palette_overlay_red2[i] = palette_blendColor(palette_overlay_red1[i], palette_overlay_red4[i], 192);

            }
            for (i = 24; i < 32; i++) {
                palette_setColor(i, palette_overlay_red1[i-24]);
            }
            // end row: 149
        } else if (frameCount == 641){
           copper_setExtentionIndex(18*13 + 2);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red2[i-24]);
            }
            copper_setExtentionIndex(18*14 + 2);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red3[i-24]);
            }
            copper_setExtentionIndex(18*15 + 2);
            for (i = 24; i < 32; i++){
                copper_setColor(i,palette_overlay_red4[i-24]);
            }
            rowY1 = 200;
            rowY2 = 201;
            rowY3 = 202;
        }else if (frameCount < 712){
            rowY1--;
            if (frameCount>660 && rowY1>160) rowY2--;
            if (frameCount>680 && rowY3>180) rowY3--;

            copper_setExtention((18*13), rowY1*256 + 7);
            copper_setExtention((18*14), rowY2*256 + 7);
            copper_setExtention((18*15), rowY3*256 + 7); 
        }else if (frameCount < 750){
           rowY1 = frameCount - 712;
           for (i = 24; i < 32; i++) {
                palette_setColor(i, palette_blendColor(palette_overlay_red1[i-24],palette_overlay_red2[i-24],rowY1<<2));
            }
        } else if (frameCount < 760){
            
        } else if (frameCount == 760){
            //copper_reset();
            //fogActive = FALSE;
            //screen_overrideBitPlane(4, 0);
            masterFade = 255;

             for (i = 0; i < 16; i++){
                palettergb[i*3] = (intro_palette2[i] & 0xf00) >> 8;
                palettergb[i*3+1] = (intro_palette2[i] & 0x0f0) >> 4;
                palettergb[i*3+2] = (intro_palette2[i] & 0x00f);
            }


        }
        if (fogActive){
            fogY++;
            if (fogY >512) fogY = 0;
            screen_overrideBitPlane(4, tempPlane + fogY * 40);
        }
        
        screen_flip();

        modTrigger = mod_E8Value();
        if (modTrigger>0 && fadeOut == 0){
            fadeOut = 1;
            copper_reset();
            fogActive = FALSE;
            screen_overrideBitPlane(4, 0);
        } 

        if (fadeOut){
            masterFade -= 6;
            if (masterFade < 0) masterFade = 0;
            setFade(masterFade,0,16);
            if (masterFade == 0){
                loop = 0;
            }
        }

    
        if (isMouseDown()){
            loop = 0;
        }
    
        if (loop == 0){
            FreeMem(tempPlane, fog->size);
            asset_free(logo_asset);
        }
    }

}