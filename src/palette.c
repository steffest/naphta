#include <exec/types.h>
#include "app.h"

#define maxColors 32

struct cycleInfo {
    UBYTE index; // number to indicate where in the color cycle we are
    UBYTE max; // max number of cycle steps;
    UBYTE speed; // speed of the color cycle (where 0 is fastest)
    UBYTE speedDelay; // number to count upwards to the speed of the color cycle
    UBYTE type; // type of fade: 0 = no fade, 1 = fade to target, 2 = cycle
    UWORD colors[100]; // array of colors to cycle through
    UWORD blendColor;
    UBYTE blend;
};

struct cycleInfo colorCycleInfo[maxColors];
static BOOL isCycling = FALSE;

UWORD rainbowColors[] = {0x448,0x349,0x548,0x548,0x359,0x459,0x748,0x459,0x559,0x848,0x659,0x459,0x36a,0x759,0x948,0x859,0xb23,0xb23,0xc23,0xc24,0xb26,0xc25,0x46a,0x959,0xa48,0xc38,0xc43,0xb38,0xa59,0x37a,0xc53,0xb59,0xe33,0xe34,0xc59,0x57b,0xe36,0xd38,0xe37,0xd48,0xe43,0x77a,0x87a,0x88b,0xc73,0xe53,0x98b,0x29c,0x89b,0xa8b,0xb8b,0xe73,0x5ad,0xb8b,0xc93,0xc8b,0x4b4,0x2ac,0x4b5,0x4b5,0x4b6,0xe88,0x4b5,0xd8b,0xe89,0x5b5,0xe8a,0xd8b,0xd8b,0x5b4,0xe8a,0x4b7,0xe8b,0x9ac,0x4b9,0x7b4,0x7b5,0x4ba,0xe98,0x4cb,0xf94,0xca3,0x7c7,0x8c4,0x8c5,0x5be,0x9bd,0x7c8,0xfa8,0x8c4,0x7ca,0x9c4,0x6ce,0x7cc,0x9c4,0xfb8,0x9c8,0x7cd,0x9c9,0xac7,0xbc4,0x9d9,0x9ce,0xbd4,0xdc4,0xada,0xbd8,0xfc3,0xadc,0x9de,0xbd8,0xade,0xadd,0xcd3,0xfc8,0xce8,0xde8,0xee2,0xfd9,0xee8,0xfe3,0xff3,0xff9};

void palette_init(void){
    UBYTE i;
    for (i = 0; i < maxColors; i++) {
        colorCycleInfo[i].type = 0;
        copper_setColorValue(i, 0);
    }
}

void palette_set(UWORD *palette, UBYTE size){
    UBYTE i;
    for (i = 0; i < size; i++) {
        copper_setColorValue(i, palette[i]);
        colorCycleInfo[i].type = 0;
    }
}

void palette_setBlack(UBYTE size){
    UBYTE i;
    for (i = 0; i < size; i++) {
        copper_setColorValue(i, 0x000);
        colorCycleInfo[i].type = 0;
    }
}

void palette_setColor(UBYTE index, UWORD color){
    copper_setColorValue(index, color);
    colorCycleInfo[index].type = 0;
}

void palette_setColorCycle(UBYTE index, UWORD *colors, UBYTE size, UBYTE speed){
    UBYTE i;
    colorCycleInfo[index].index = 0;
    colorCycleInfo[index].max = size - 1;
    colorCycleInfo[index].speed = speed;
    colorCycleInfo[index].speedDelay = 0;
    colorCycleInfo[index].type = 2;
    if (size > 100) size = 100;
    for (i = 0; i < size; i++) {
        colorCycleInfo[index].colors[i] = colors[i];
    }
}

void palette_updateColorCycle(void){
    UBYTE i;
    UWORD color;
    
    if (!isCycling) return;
    for (i = 0; i < maxColors; i++) {
        if (colorCycleInfo[i].type == 0) continue;
        colorCycleInfo[i].speedDelay++;
        if (colorCycleInfo[i].speedDelay > colorCycleInfo[i].speed) {
            colorCycleInfo[i].speedDelay = 0;
            colorCycleInfo[i].index++;
            if (colorCycleInfo[i].index > colorCycleInfo[i].max) colorCycleInfo[i].index = 0;

            color = colorCycleInfo[i].colors[colorCycleInfo[i].index];
            if (colorCycleInfo[i].blend) {
                color = palette_blendColor(color, colorCycleInfo[i].blendColor, colorCycleInfo[i].blend);
            }

            copper_setColorValue(i, color);
    
            //copper_setColorValue(i, colorFades[i][colorFadeIndex[i]]);
        }
    }
}

void palette_stopColorCycle(void){
    isCycling = FALSE;
}

void palette_startColorCycle(void){
    isCycling = TRUE;
}

void palette_setColorCycleIndex(UBYTE colorIndex,UBYTE cycleIndex){
    if (cycleIndex > colorCycleInfo[colorIndex].max) cycleIndex = 0;
    colorCycleInfo[colorIndex].index = cycleIndex;
}

void palette_fluctuateColor(UBYTE index, UWORD startColor, BYTE min, BYTE max, UBYTE speed, BYTE startDirection){
    UBYTE startRed = (startColor & 0xF00) >> 8;
    UBYTE startGreen = (startColor & 0xF0) >> 4;
    UBYTE startBlue = startColor & 0xF;
    UBYTE red = startRed;
    UBYTE green = startGreen;
    UBYTE blue = startBlue;
    BYTE offset = 0;
    BYTE direction = startDirection;
    UBYTE i;
    UBYTE steps = (max - min) * 2;

    colorCycleInfo[index].colors[0] = startColor;

    for (i = 1; i < steps; i++) {
        offset += direction;
        red = startRed + offset;
        if (red > 15) red = 15;
        if (red < 0) red = 0;
        green = startGreen + offset;
        if (green > 15) green = 15;
        if (green < 0) green = 0;
        blue = startBlue + offset;
        if (blue > 15) blue = 15;
        if (blue < 0) blue = 0;
        if (offset >= max) direction = -1;
        if (offset <= min) direction = 1;
        
        colorCycleInfo[index].colors[i] = (red << 8) + (green << 4) + blue;
    }
    colorCycleInfo[index].index = 0;
    colorCycleInfo[index].max = steps - 1;
    colorCycleInfo[index].speed = speed;
    colorCycleInfo[index].speedDelay = 0;
    colorCycleInfo[index].type = 2;
}

void palette_removeColorCycle(UBYTE index){
    colorCycleInfo[index].type = 0;
}

void palette_addColorCycle(UBYTE index, UWORD startColor){
    colorCycleInfo[index].colors[0] = startColor;
    colorCycleInfo[index].type = 2;
}

void palette_setBlendColor(UBYTE index, UWORD color, UBYTE blend){
    colorCycleInfo[index].blendColor = color;
    colorCycleInfo[index].blend = blend;
}

UWORD palette_blendColor(UWORD color1, UWORD color2, UBYTE blend){
    UBYTE red1 = (color1 & 0xF00) >> 8;
    UBYTE green1 = (color1 & 0xF0) >> 4;
    UBYTE blue1 = color1 & 0xF;
    UBYTE red2 = (color2 & 0xF00) >> 8;
    UBYTE green2 = (color2 & 0xF0) >> 4;
    UBYTE blue2 = color2 & 0xF;
    UBYTE red = red1 + ((red2 - red1) * blend >> 8);
    UBYTE green = green1 + ((green2 - green1) * blend >> 8);
    UBYTE blue = blue1 + ((blue2 - blue1) * blend >> 8);
    return (red << 8) + (green << 4) + blue;

    //return color1;
}

UWORD palette_blendRGB(UBYTE red1, UBYTE green1, UBYTE blue1, UBYTE red2, UBYTE green2, UBYTE blue2, UBYTE blend){
    UBYTE red = red1 + ((red2 - red1) * blend >> 8);
    UBYTE green = green1 + ((green2 - green1) * blend >> 8);
    UBYTE blue = blue1 + ((blue2 - blue1) * blend >> 8);
    return (red << 8) + (green << 4) + blue;
}

UWORD palette_randomBrightColor(void){
    UBYTE index = rand() % 100;
    return rainbowColors[index];
}

UWORD palette_getBrightColor(UBYTE index){
    return rainbowColors[index];
}
