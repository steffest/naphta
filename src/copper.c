#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include "app.h"

#define BG_COLOR				(0x000)
#define CENTER_LINE				(150)

// copper instruction macros
#define COP_MOVE(addr, data) addr, data
#define COP_WAIT(loc, mask) loc, mask

// playfield control
// single playfield, 5 bitplanes (32 colors)
#define BPLCON0_VALUE (0x5200)
// http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0092.html
// bits 14,13,12 define the number of bitplanes

// 0x0048: playfield 2 has priority over sprites (bit 6)
// 0x0020: default mode ?
// see http://amigadev.elowar.com/read/ADCD_2.1/Hardware_Manual_guide/node0159.html
#define BPLCON2_VALUE (0x0020)

#define DIWSTRT_VALUE      0x2c81
#define DIWSTOP_VALUE_PAL  0x2cc1
#define DIWSTOP_VALUE_NTSC 0xf4c1

// Data fetch
#define DDFSTRT_VALUE      0x0038
#define DDFSTOP_VALUE      0x00d0

// copper list indexes
#define COPLIST_IDX_SPR0_PTH_VALUE (3)
#define COPLIST_IDX_DIWSTOP_VALUE (COPLIST_IDX_SPR0_PTH_VALUE + 32 + 6)
#define COPLIST_IDX_BPL1MOD_VALUE (COPLIST_IDX_DIWSTOP_VALUE + 6)
#define COPLIST_IDX_BPL2MOD_VALUE (COPLIST_IDX_BPL1MOD_VALUE + 2)
#define COPLIST_IDX_COLOR00_VALUE (COPLIST_IDX_BPL2MOD_VALUE + 2)
#define COPLIST_IDX_BPL1PTH_VALUE (COPLIST_IDX_COLOR00_VALUE + 64)

#define COPLIST_EXTENTION (COPLIST_IDX_BPL1PTH_VALUE + 19)
#define COPLIST_IDX_COLOR_VALUE (COPLIST_IDX_BPL1PTH_VALUE + 22)

static UWORD __chip copperList[7000];

extern struct Custom far custom;
extern struct GfxBase *GfxBase;

UWORD palFixPositon = 0;
UWORD extentionIndex = 0;

void copper_init(){    
    UWORD i = 0;
    UWORD index = 0;
    UWORD value = 0;
    UWORD max = sizeof(copperList) / sizeof(UWORD);
    
    //for (i = 0; i < max; i++) copperList[i] = 0;

    copperList[0] = 0x1fc; // FMODE slow fetch mode, AGA compatibility (no idea what this is but all the fancy code examples have it)
    copperList[1] = 0;

    // sprites first
    index = 2;
    value = SPR0PTH;
    for (i = 0; i < 16; i++){
        copperList[index] = value;
        index += 2;
        value += 2;
    }


    copperList[index++] = DDFSTRT;
    copperList[index++] = DDFSTRT_VALUE;
    copperList[index++] = DDFSTOP;  
    copperList[index++] = DDFSTOP_VALUE;
    copperList[index++] = DIWSTRT;
    copperList[index++] = DIWSTRT_VALUE;
    copperList[index++] = DIWSTOP;
    copperList[index++] = DIWSTOP_VALUE_PAL;
    copperList[index++] = BPLCON0;
    copperList[index++] = BPLCON0_VALUE;
    copperList[index++] = BPLCON2;
    copperList[index++] = BPLCON2_VALUE;
    copperList[index++] = BPL1MOD;
    copperList[index++] = 0;
    copperList[index++] = BPL2MOD;
    copperList[index++] = 0;

    // set up the display colors
    for (i = 0; i < 32; i++){
        copperList[index++] = COLOR00 + (i << 1);
        copperList[index++] = 0;
    }

    // bitplane pointers
    value = BPL1PTH;
    for (i = 0; i < (SCREENDEPTH*2); i++){
        copperList[index++] = value;
        copperList[index++] = 0;
        value += 2;
    }

    // PAL fix
    copperList[index++] = 0xffdf;
    copperList[index++] = 0xfffe;

    // end copperlist
    copperList[index++] = 0xffff;
    copperList[index++] = 0xfffe;

    if (index-4 != COPLIST_EXTENTION){
        printf("ERROR: index != COPLIST_EXTENTION\n");
    }


}

void copper_reset(void){
  copperList[COPLIST_EXTENTION] = 0xffdf;
  copperList[COPLIST_EXTENTION+1] = 0xfffe;
  // end copperlist
  copperList[COPLIST_EXTENTION+2] = 0xffff;
  copperList[COPLIST_EXTENTION+3] = 0xfffe;
}

void copper_activate(){
   custom.cop1lc = copperList;
}

void copper_restore(){
    custom.cop1lc = (unsigned long) ((struct GfxBase *) GfxBase)->copinit;
}

void copper_setColorValue(unsigned short index, unsigned short value){
    copperList[COPLIST_IDX_COLOR00_VALUE + (index << 1)] = value;
}

void copper_setBitPlane(unsigned short index, unsigned short value){
    copperList[COPLIST_IDX_BPL1PTH_VALUE + index] = value;
}

void copper_setSpritePointer(unsigned short *sprite, char spriteIndex){
    copperList[COPLIST_IDX_SPR0_PTH_VALUE + (spriteIndex << 2)] = (((ULONG) sprite) >> 16) & 0xffff;
    copperList[COPLIST_IDX_SPR0_PTH_VALUE + (spriteIndex << 2) + 2] = ((ULONG) sprite) & 0xffff;
}

void copper_setValue(unsigned short index, unsigned short value){
    copperList[COPLIST_IDX_COLOR_VALUE + index] = value & 0xffff;
}

void copper_setExtention(unsigned short index, unsigned short value){
    copperList[COPLIST_EXTENTION + index] = value & 0xffff;
}

void copper_start(void){
    extentionIndex = COPLIST_EXTENTION;
}

void copper_setExtentionIndex(UWORD index){
    extentionIndex = COPLIST_EXTENTION + index;
}

UWORD copper_waitForLine(UWORD line){
    copperList[extentionIndex++] = line*256 + 7;
    copperList[extentionIndex++] = 0xfffe;
    return extentionIndex-2;
}

UWORD copper_setColor(UWORD index, UWORD color){
    copperList[extentionIndex++] = 0x180 + (index << 1);
    copperList[extentionIndex++] = color;
    return extentionIndex-2;
}

void copper_setLineSection(UWORD line){
    copperList[extentionIndex++] = line*256 + 103;
    copperList[extentionIndex++] = 0xfffe;
    copperList[extentionIndex++] = 0x180;
    copperList[extentionIndex++] = 0;

    copperList[extentionIndex++] = line*256 + 167;
    copperList[extentionIndex++] = 0xfffe;
    copperList[extentionIndex++] = 0x180;
    copperList[extentionIndex++] = 0;
}

UWORD copper_end(){
    copperList[extentionIndex++] = 0xffff;
    copperList[extentionIndex++] = 0xfffe;
    return extentionIndex-2;
}

void copper_setBackBlock(UWORD color,UWORD color2,UWORD color3,UWORD color4,UWORD color5,UWORD color6){
    UBYTE index;
    for (index = 0; index < 90; index++){
        copperList[COPLIST_IDX_COLOR_VALUE + (index << 3)] = color;
    }
    for (index = 90; index < 100; index++){
        copperList[COPLIST_IDX_COLOR_VALUE + (index << 3)] = color2;
    }
    for (index = 100; index < 110; index++){
        copperList[COPLIST_IDX_COLOR_VALUE + (index << 3)] = color3;
    }
     for (index = 110; index < 120; index++){
        copperList[COPLIST_IDX_COLOR_VALUE + (index << 3)] = color4;
    }
     for (index = 120; index < 130; index++){
        copperList[COPLIST_IDX_COLOR_VALUE + (index << 3)] = color5;
    }
     for (index = 130; index < 134; index++){
        copperList[COPLIST_IDX_COLOR_VALUE + (index << 3)] = color6;
    }
    

}


/*void copper_setGrid(void){
    UWORD lineHeight = 16;
    UWORD startLine = 28 + lineHeight;
    UWORD index = COPLIST_EXTENTION;
    UBYTE i;
    UBYTE linecount = 0;

    UBYTE r = 0;

    while(linecount<16){
        
        copperList[index++] = startLine*256 + 7;
        copperList[index++] = 0xfffe;
        
        copperList[index++] = 0x180;
        copperList[index++] = (r<<8);
        copperList[index++] = 0x184;
        copperList[index++] = (r<<8) | 2;
        copperList[index++] = 0x188;
        copperList[index++] = (r<<8) | 4;
        copperList[index++] = 0x18c;
        copperList[index++] = (r<<8) | 6;
        copperList[index++] = 0x190;
        copperList[index++] = (r<<8) | 8;
        copperList[index++] = 0x194;
        copperList[index++] = (r<<8) | 10;
        copperList[index++] = 0x198;
        copperList[index++] = (r<<8) | 12;
        copperList[index++] = 0x19c;
        copperList[index++] = (r<<8) | 14;

        copperList[index++] = (startLine+4)*256 + 7;
        copperList[index++] = 0xfffe;

        copperList[index++] = 0x182;
        copperList[index++] = (r<<8) | 1;
        copperList[index++] = 0x186;
        copperList[index++] = (r<<8) | 3;
        copperList[index++] = 0x18a;
        copperList[index++] = (r<<8) | 5;
        copperList[index++] = 0x18e;
        copperList[index++] = (r<<8) | 7;
        copperList[index++] = 0x192;
        copperList[index++] = (r<<8) | 9;
        copperList[index++] = 0x196;
        copperList[index++] = (r<<8) | 11;
        copperList[index++] = 0x19a;
        copperList[index++] = (r<<8) | 13;
        copperList[index++] = 0x19e;
        copperList[index++] = (r<<8) | 15;


        r ++;
        linecount++;
        startLine += lineHeight;
        if (startLine>255){
            palFixPositon = index;
            startLine -= 256;
            copperList[index] = 0xffdf;
            index++;
            copperList[index] = 0xfffe;
            index++;
        }
    }
    

    // end copperlist
    copperList[index] = 0xffff;
    index++;
    copperList[index] = 0xfffe;    


    //printf("index %d -max  %d\n", index, sizeof(copperList) / sizeof(UWORD));  

   //printf("copper value %d\n", copperList[COPLIST_EXTENTION]);
   //printf("copper value %d\n", gridCopper[index]);
}*/

UWORD generateGridLine(UWORD index, UWORD line, UBYTE r){
    UBYTE i;
    copperList[index++] = (line)*256 + 7;
    copperList[index++] = 0xfffe;

    for (i = 1; i < 16; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }   

    return index;
}

UWORD setGridLine(UWORD index, UWORD screenIndex, UWORD *screen){
    UBYTE i;
    index += 2;

    for (i = 0; i < 15; i++){
        index++;
        copperList[index++] = screen[screenIndex++];
    }   

    return index;
}

UWORD generateGridLineToggle(UWORD index, UWORD line, UBYTE r, UBYTE r2){
    UBYTE i;
    copperList[index++] = (line)*256 + 7;
    copperList[index++] = 0xfffe;

    for (i = 1; i < 8; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }   

    for (i = 9; i < 16; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }

    copperList[index++] = line*256 + 7 + 90; 
    copperList[index++] = 0xfffe;
    
    for (i = 1; i < 5; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }

    for (i = 8; i < 12; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }


    copperList[index++] = (line)*256 + 7 + 136;
    copperList[index++] = 0xfffe;
    
    for (i = 5; i < 9; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }
    for (i = 12; i < 16; i++){
        copperList[index++] = 0x180 + (i << 1);
        copperList[index++] = 0;
    }

    return index;
}

UWORD setGridLineToggle(UWORD index, UWORD screenIndex, UWORD screenIndex2, UWORD *screen){
    UBYTE i;
    index += 2;

    for (i = 0; i < 7; i++){
        index++;
        copperList[index++] = screen[screenIndex + i];
    }   

    for (i = 8; i < 15; i++){
        index++;
        copperList[index++] = screen[screenIndex2 + (i-8)];
    }

    index += 2;
    for (i = 0; i < 4; i++){
        index++;
        copperList[index++] = screen[screenIndex2 + (i+7)];
    }

    for (i = 7; i < 11; i++){
       index++;
       copperList[index++] = screen[screenIndex + i];
    }

    index += 2;
    for (i = 4; i < 8; i++){
        index++;
        copperList[index++] = screen[screenIndex2 + (i+7)];
    }
    for (i = 11; i < 15; i++){
        index++;
        copperList[index++] =screen[screenIndex + i];
    }

    return index;
}

void copper_setGrid16(void){
    UWORD startLine = 44;
    UWORD index = COPLIST_EXTENTION;
    UBYTE i;
    UBYTE linecount = 0;
    UBYTE r = 0;

    while(linecount<16){
        if (linecount == 0){
            index = generateGridLine(index, startLine++, 0);
            startLine+=3;
        } else {
            index = generateGridLineToggle(index, startLine++, 0, 0);
            index = generateGridLineToggle(index, startLine++, 0, 0);
            index = generateGridLineToggle(index, startLine++, 0, 0);
            index = generateGridLine(index, startLine++, 0);
        }

        if (linecount < 15){
            startLine += 9;
            
            if (startLine>255){
                palFixPositon = index;
                startLine -= 256;
                copperList[index++] = 0xffdf;
                copperList[index++] = 0xfffe;
            }
            
            index = generateGridLineToggle(index, startLine++, 0, 0);
            index = generateGridLineToggle(index, startLine++, 0, 0);
            index = generateGridLineToggle(index, startLine++, 0, 0);
        }

        r++;
        linecount++;
    }

    
    // end copperlist
    copperList[index] = 0xffff;
    index++;
    copperList[index] = 0xfffe;    

}

void copper_setGrid32(void){
    UWORD lineHeight = 5;
    UWORD index = COPLIST_EXTENTION;
    UWORD startLine = 46;
    UBYTE i;
    UBYTE linecount = 0;
    UBYTE r = 0;

    while(linecount<52){
        copperList[index++] = startLine*256 + 5;
        copperList[index++] = 0xfffe;

        for (i = 1; i < 15; i++){
            copperList[index++] = 0x180 + (i << 1);
            copperList[index++] = 0;
            copperList[index++] = 0x180 + ((i+14) << 1);
            copperList[index++] = 0x000;
        }

        startLine += lineHeight;
        if (linecount%4 == 2) startLine--;
        if (linecount%4 == 3) startLine++;

        r++;
        linecount++;

        if (startLine>255){
            palFixPositon = index;
            startLine -= 256;
            copperList[index++] = 0xffdf;
            copperList[index++] = 0xfffe;
        }
    }

    // end copperlist
    copperList[index] = 0xffff,0xfffe;    

}

// deprecated ?
void copper_setGrid2(void){
    UWORD lineHeight = 16;
    UWORD startLine = 28 + lineHeight;
    UWORD index = COPLIST_EXTENTION;
    UBYTE i;
    UBYTE linecount = 0;

    UBYTE r = 0;

    while(linecount<16){
        copperList[index] = startLine*256 + 7;
        index++;
        copperList[index] = 0xfffe;
        index++;

        for (i = 0; i < 16; i++){
            copperList[index] = 0x180 + (i << 1);
            index++;
            copperList[index] = (r<<8) | i;
            index++;
        }

        r ++;
        linecount++;
        startLine += lineHeight;
        if (startLine>255){
            palFixPositon = index;
            startLine -= 256;
            copperList[index] = 0xffdf;
            index++;
            copperList[index] = 0xfffe;
            index++;
        }
    }
    

    // end copperlist
    copperList[index] = 0xffff;
    index++;
    copperList[index] = 0xfffe;    


    //printf("index %d -max  %d\n", index, sizeof(copperList) / sizeof(UWORD));
/*
    // fill the rest with 0;
    for (index = upper; index < max; index++){
        copperList[COPLIST_EXTENTION + index] = 0;
    }*/

  

   //printf("copper value %d\n", copperList[COPLIST_EXTENTION]);
   //printf("copper value %d\n", gridCopper[index]);
}

void copper_setGridColor(UBYTE g){
    UBYTE i;
    UBYTE linecount = 0;
    UWORD index = COPLIST_IDX_COLOR00_VALUE;
    UBYTE r = 0;

    for (i = 0; i < 16; i++){
        copperList[index] = (r << 8) | (g << 4) | i;
        index += 2;
    }
    

    r++;
    index = COPLIST_EXTENTION;
    while (linecount < 16){
        index += 2;
        for (i = 0; i < 16; i++){
            index ++;
            copperList[index] = (r << 8) | (g << 4) | i;
            index ++;
        }
        linecount++;
        r++;
        if (index == palFixPositon){
            index += 2;
        }
    }
}

void copper_setGridColors(UWORD *screen){
    UBYTE linecount = 0;
    UBYTE i;
    UWORD index = COPLIST_EXTENTION;
    UWORD screenIndex;
    UBYTE r = 0;
    UBYTE g = 0;
    UBYTE b = 0;

    linecount++;
    while (linecount < 16){
        index += 2;
        for (i = 0; i < 16; i++){
            index ++;
            screenIndex = (i + (linecount << 4))*3;
            r = screen[screenIndex];
            g = screen[screenIndex + 1];
            b = screen[screenIndex + 2];
            copperList[index] = (r << 8) | (g << 4) | b;
            index ++;
        }
        linecount++;
        if (index == palFixPositon){
            index += 2;
        }
    }
}

void copper_setGrid2Colors(UBYTE *screen){
    UBYTE linecount = 0;
    UBYTE i;
    UWORD index = COPLIST_EXTENTION;
    UWORD p;
    UWORD color;
    UWORD screenIndex;
    UBYTE r = 0;
    UBYTE g = 0;
    UBYTE b = 0;
    UBYTE isTop;
    UBYTE isBottom;
    BYTE brightness = 0;
    UBYTE isEven = 0;
    UBYTE isOdd = 0;
    UBYTE col;


    while (linecount < 25){
        if (index == palFixPositon) index += 2;    
        index += 2;

        isEven = linecount%2 == 0;
        isOdd = !isEven;

        for (i = 0; i < 14; i++){
            p = index;
            screenIndex = (i + (linecount*14))*3;
            r = screen[screenIndex];
            g = screen[screenIndex + 1];
            b = screen[screenIndex + 2];

            col = i%2;
            isTop = (isEven && col) || (isOdd && col == 0);
            isBottom = (isEven && col == 0) || (isOdd && col);

            color= r << 8 | g << 4 | b;
            
            copperList[p+1] = 
            isTop ?
                (clampNibbleUp(r+2) << 8) | 
                (clampNibbleUp(g+2) << 4) | 
                clampNibbleUp(b+2)
                : color;

            brightness = isTop ? 4 : 1;
            copperList[p+3] = 
            (clampNibbleUp(r+brightness) << 8) | 
            (clampNibbleUp(g+brightness) << 4) | 
            clampNibbleUp(b+brightness);


            copperList[p+59] = 
            isBottom ?
                (clampNibbleDown(r-2) << 8) | 
                (clampNibbleDown(g-2) << 4) | 
                clampNibbleDown(b-2)
                : color;

            copperList[p+61] = 
            isBottom ?
                color
                : (clampNibbleUp(r+2) << 8) | 
                (clampNibbleUp(g+2) << 4) | 
                clampNibbleUp(b+2);

            index += 4;
        }

        index += 58;
        linecount++;
    }
}

void copper_setGridScreen(UWORD *screen){
    UBYTE linecount = 0;
    UBYTE i;
    UWORD index = COPLIST_EXTENTION;
    UWORD screenIndex;

    while (linecount < 16){
        screenIndex = linecount * 15;
        
        if (linecount == 0){
            index = setGridLine(index, screenIndex, screen);
        } else {
            index = setGridLineToggle(index, screenIndex, screenIndex-15, screen);
            index = setGridLineToggle(index, screenIndex, screenIndex-15, screen);
            index = setGridLineToggle(index, screenIndex, screenIndex-15, screen);
            index = setGridLine(index, screenIndex, screen);
        }

        if (linecount < 15){
            if (index == palFixPositon){
                index += 2;
            }
            index = setGridLineToggle(index, screenIndex, screenIndex+15, screen);
            index = setGridLineToggle(index, screenIndex, screenIndex+15, screen);
            index = setGridLineToggle(index, screenIndex, screenIndex+15, screen);
        }

        linecount++; 
    }
}

void copper_setGridScreen1b(UWORD *screen){
    UBYTE linecount = 0;
    UBYTE i;
    UWORD index = COPLIST_EXTENTION;
    UWORD screenIndex;
    while (linecount < 16){
        index += 2;
        for (i = 0; i < 8; i++){
            index ++;
            screenIndex = (i + (linecount << 4));
            copperList[index] = screen[screenIndex];
            index ++;
        }

        index += 2;

        for (i = 0; i < 8; i++){
            index ++;
            screenIndex = ((i+8) + (linecount << 4));
            copperList[index] = screen[screenIndex];
            index ++;
        }


        linecount++;
        if (index == palFixPositon){
            index += 2;
        }
    }
}

void copper_setGridValue(UWORD value, UWORD index){
    copperList[COPLIST_IDX_COLOR_VALUE + index] = value;
}