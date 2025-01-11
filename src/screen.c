#include <exec/memory.h>
#include <hardware/custom.h>
#include "app.h"

#define SPACE 0x40
#define LF_COOKIE_CUT 0xca
#define LF_XOR 0x4a

extern struct Custom far custom;

/*
    Note: this is not an intuition screen,
    it's just a chip memory buffer that we use to draw to.
*/

static struct DScreen screen;
static UWORD __chip scratchmem[12];
ULONG overideBitPlanes[] = {0,0,0,0,0};


// copies the current screen buffer to the other buffer
// (which means, copy the back buffer to the front buffer)
void screen_copyToBuffer(void){

    // TODO: performance test:
    // - use memcpy
    // - use CopyMemQuick
    // - use blitter

    if (screen.doubleBuffer){
        if (screen.activeScreen == 0){
            //memcpy(screen.canvas2, screen.canvas, screen.size);
            CopyMemQuick(screen.canvas2, screen.canvas, screen.size);
        } else {
            CopyMemQuick(screen.canvas, screen.canvas2, screen.size);
            //memcpy(screen.canvas, screen.canvas2, screen.size);
        }
    }
}

void screen_flip(void){
    UBYTE i;
    unsigned long addr;
    UBYTE *canvas;

    if (!screen.doubleBuffer){
        return;
    }

    screen.activeScreen = screen.activeScreen == 0 ? 1 : 0;
    canvas = screen.activeScreen == 0 ? screen.canvas : screen.canvas2;

    for (i = 0; i < screen.depth; i++) {
        addr = (unsigned long) (canvas + (i * screen.planeSize));

        //addr = (unsigned long) &(screen.canvas2[i * screen.planeSize]);

        if (overideBitPlanes[i]){
            addr = overideBitPlanes[i];
        }
        copper_setBitPlane(i*4,(addr >> 16) & 0xffff);
        copper_setBitPlane(i*4 + 2,addr & 0xffff);
    }
}

void screen_disableDoubleBuffer(void){
    if (screen.activeScreen == 1){
        screen_flip();
    }
    screen.doubleBuffer = 0;
}

void screen_enableDoubleBuffer(void){
    if (screen.canvas2) screen.doubleBuffer = 1;
}

char screen_init(USHORT width, USHORT height, UBYTE bitplaneCount, BOOL doubleBuffer){
    unsigned long addr;
    UBYTE i;

    screen.planeSize = (width >> 3) * height;
    screen.size = screen.planeSize * bitplaneCount;
    screen.canvas = AllocMem(screen.size, MEMF_CHIP | MEMF_CLEAR); 
    screen.width = width;
    screen.height = height;
    //printf("Screen size %d\n",screen.size);
    screen.depth = bitplaneCount;
    screen.doubleBuffer = doubleBuffer;
    screen.activeScreen = 0;

    // let's keep it into 1 big memory block, so we can draw same-size full color images with a single blit?

    if (screen.canvas == NULL) {
        printf("Screen: Failed to allocate chip memory for screen buffer\n");
        return -1;
    }

    if (screen.doubleBuffer) {
        screen.canvas2 = AllocMem(screen.size, MEMF_CHIP | MEMF_CLEAR);
        if (screen.canvas2 == NULL) {
            FreeMem(screen.canvas, screen.size);
            printf("Screen: Failed to allocate chip memory for screen buffer 2\n");
            return -1;
        }
    }


    for (i = 0; i < bitplaneCount; i++) {
        addr = (APTR) (screen.canvas + (i * screen.planeSize));
        //addr = (unsigned long) &(screen.canvas[i * screen.planeSize]);
        copper_setBitPlane(i*4,(addr >> 16) & 0xffff);
        copper_setBitPlane(i*4 + 2,addr & 0xffff);
    }

    palette_init();
    
    return 0;
}

void screen_close(void){
    if (screen.canvas){
        FreeMem(screen.canvas, screen.size);
    }
    if (screen.canvas2){
        FreeMem(screen.canvas2, screen.size);
    }
}

void screen_clear(void){
  /*WaitBlit();
  custom.bltcon0 = BLTCON0_USED; // only D channel; 0X0100
  custom.bltcon1 = 0;
  custom.bltdmod = 0; 
  custom.bltdpt = getScreenCanvas(); // destination pointer
  custom.bltsize = ((screen.height << 6) | screen.width >> 4) * ; */

  UBYTE i;
  for (i = 0; i < screen.depth; i++) {
    bitplane_clear(i);
  }
}

void screen_drawPixel(USHORT x, USHORT y, UBYTE color){
    USHORT planeOffset = (y * screen.width + x) >> 3;
    UBYTE bitOffset = (y * screen.width + x) & 7;
    UBYTE plane;
    UBYTE mask;
    UBYTE *canvas = screen_getCanvas();


    for (plane = 0; plane < screen.depth; plane++) {
        mask = 1 << (7 - bitOffset);
        if (color & (1 << plane)) {
            canvas[plane * screen.planeSize + planeOffset] |= mask;
        } else {
            canvas[plane * screen.planeSize + planeOffset] &= ~mask;
        }
    }
}

void screen_drawRect(USHORT x, USHORT y, USHORT width, USHORT height, UBYTE color){
    UBYTE plane;
    UBYTE isPlaneActive;
    for (plane = 0; plane < screen.depth; plane++) {
        isPlaneActive = color & (1 << plane);
        bitplane_drawRect(plane, x, y, width, height, isPlaneActive);
    } 
}

void screen_drawImage(USHORT index, USHORT x, USHORT y){
    UBYTE *data = getAssetData(index);
    USHORT imageWidth = 320;
    USHORT byte_width = imageWidth>>3;
    USHORT imageHeight = 256;
    USHORT bX = 0;
    USHORT bY = 0;
    UBYTE bit = 0;
    UBYTE byte = 0;
    UBYTE *canvas = screen_getCanvas();


    int tile_width_pixels;
    int tile_x0;
    int blit_width;
    int blit_width0_pixels;
    int src_blit_width;
    int dst_x0;
    int dst_shift;
    int dst_blit_width;
    int dst_offset;
    int final_blit_width;
    USHORT srcmod;
    USHORT dstmod;
    USHORT bltsize;
    int srcx;
    int srcy;
    int bobs_plane_size;
    int bg_plane_size;
    char *src;
    char *mask;
    char *dst;
    int i;
    USHORT alwm;

    int dstx = x; 
    int dsty = y; 

    tile_width_pixels = 320;
    tile_x0 = 0;
    blit_width = tile_width_pixels / 16;

     // width not a multiple of 16 ? -> add 1 to the width
    if (tile_width_pixels & 0x0f) blit_width++;

    blit_width0_pixels = blit_width * 16;
    
    src_blit_width = blit_width;
    if (tile_x0 > blit_width0_pixels - tile_width_pixels) src_blit_width++;

    // 2. Determine the amount of shift and the first word in the destination
    dst_x0 = dstx & 0x0f;  // destination x relative to the containing word
    dst_shift = dst_x0 - tile_x0;  // shift amount
    dst_blit_width = blit_width;
    dst_offset = 0;

    // negative shift => shift is to the left, so we extend the shift to the
    // left and right-shift in the previous word so we always right-shift
    if (dst_shift < 0) {
        dst_shift = 16 + dst_shift;
        dst_blit_width++;
        dst_offset = -2;
    }

    // make the blit wider if it needs more space
    if (dst_x0 > blit_width0_pixels - tile_width_pixels) {
        dst_blit_width++;
    }

    alwm = 0xffff;
    final_blit_width = src_blit_width;

    // due to relative positioning and shifts, the destination blit width
    // can be larger than the source blit, so we use the larger of the 2
    // and mask out last word of the source
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }

    WaitBlit();

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    //custom.bltcon0 = 0x0fca | (dst_shift << 12);
    
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);     // enable channels A and D, LF => D = A
    
    custom.bltcon1 = dst_shift << 12;

    srcmod = imageWidth / 8 - (final_blit_width * 2);
    dstmod = screen.width / 8 - (final_blit_width * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    bltsize = (imageHeight << 6) | (final_blit_width & 0x3f);

    srcx = 0;
    srcy = 0;

    bobs_plane_size = imageWidth / 8 * imageHeight;
    bg_plane_size = screen.width / 8 * screen.height;

    src = data + srcy * imageWidth / 8 + srcx / 8;
    // The mask data is the plane after the source image planes
    // mask = data + srcy * 16 / 8 + srcx / 8;
    dst = canvas + dsty * screen.width / 8 + dstx / 8 + dst_offset;

    for (i = 0; i < 3; i++) {

        custom.bltapt = src; // or mask ?
        custom.bltbpt = src;
        custom.bltcpt = dst;
        custom.bltdpt = dst;
        custom.bltsize = bltsize;

        // Increase the pointers to the next plane
        src += bobs_plane_size;
        dst += bg_plane_size;

        WaitBlit();
    }

}


void screen_drawAsset2(BYTE index, USHORT x, USHORT y){
    struct ASSET asset = *getAsset(index);
    UBYTE *data = getAssetData(index);

    USHORT bX = 0;
    USHORT bY = 0;
    UBYTE bit = 0;
    UBYTE byte = 0;

    int tile_width_pixels;
    int tile_x0;
    int blit_width;
    int blit_width0_pixels;
    int src_blit_width;
    int dst_x0;
    int dst_shift;
    int dst_blit_width;
    int dst_offset;
    int final_blit_width;
    USHORT srcmod;
    USHORT dstmod;
    USHORT bltsize;
    int srcx;
    int srcy;
    int bg_plane_size;
    char *src;
    char *mask;
    char *dst;
    int i;
    USHORT alwm;

    int dstx = x; 
    int dsty = y; 

    tile_width_pixels = asset.width;
    tile_x0 = 0;
    blit_width = tile_width_pixels / 16;

     // width not a multiple of 16 ? -> add 1 to the width
    if (tile_width_pixels & 0x0f) blit_width++;

    blit_width0_pixels = blit_width * 16;
    
    src_blit_width = blit_width;
    if (tile_x0 > blit_width0_pixels - tile_width_pixels) src_blit_width++;

    // 2. Determine the amount of shift and the first word in the destination
    dst_x0 = dstx & 0x0f;  // destination x relative to the containing word
    dst_shift = dst_x0 - tile_x0;  // shift amount
    dst_blit_width = blit_width;
    dst_offset = 0;

    // negative shift => shift is to the left, so we extend the shift to the
    // left and right-shift in the previous word so we always right-shift
    if (dst_shift < 0) {
        dst_shift = 16 + dst_shift;
        dst_blit_width++;
        dst_offset = -2;
    }

    // make the blit wider if it needs more space
    if (dst_x0 > blit_width0_pixels - tile_width_pixels) {
        dst_blit_width++;
    }

    alwm = 0xffff;
    final_blit_width = src_blit_width;

    // due to relative positioning and shifts, the destination blit width
    // can be larger than the source blit, so we use the larger of the 2
    // and mask out last word of the source
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }

    WaitBlit();

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    //custom.bltcon0 = 0x0fca | (dst_shift << 12);
    
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);     // enable channels A and D, LF => D = A
    
    custom.bltcon1 = dst_shift << 12;

    srcmod = asset.width / 8 - (final_blit_width * 2);
    dstmod = SCREENWIDTH / 8 - (final_blit_width * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    bltsize = (asset.height << 6) | (final_blit_width & 0x3f);

    srcx = 0;
    srcy = 0;


    bg_plane_size = screen.width / 8 * screen.height;

    src = data + srcy * (asset.width>>3) + (srcx>>3);
    // The mask data is the plane after the source image planes
    // mask = data + srcy * 16 / 8 + srcx / 8;
    dst = screen_getCanvas() + dsty * SCREENWIDTH / 8 + dstx / 8 + dst_offset;


    for (i = 0; i < asset.planeCount; i++) {

        custom.bltapt = src; 
        custom.bltbpt = src;
        custom.bltcpt = dst;
        custom.bltdpt = dst;
        custom.bltsize = bltsize;

        // Increase the pointers to the next plane
        src += asset.planeSize;
        dst += screen.planeSize;

        WaitBlit();
    }

}

void screen_drawLine_on_plane(USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE plane){
    USHORT dx = abs(x2 - x1);
    USHORT dy = abs(y2 - y1);
    USHORT dmin, dmax;
    USHORT bytes_per_line = screen.width >> 3;
    UBYTE code;
    short aptlval;
    USHORT startx;
    USHORT texture;
    USHORT sign;
    USHORT bltcon1val;

    USHORT line_pattern = 0xffff;
    UBYTE pattern_offset = 0;
    UBYTE lf_byte = LF_XOR;
    UBYTE single = 1;
    UBYTE omit_first_pixel = 1;
    UWORD *start_address;
    UBYTE *canvas = screen_getCanvas();

    if (y1 >= y2) {
        if (x1 <= x2) {
            code = dx >= dy ? 6 : 1;
        } else {
            code = dx <= dy ? 3 : 7;
        }
    } else {
        if (x1 >= x2) {
            code = dx >= dy ? 5 : 2;
        } else {
            code = dx <= dy ? 0 : 4;
        }
    }

    if (dx <= dy) {
        dmin = dx;
        dmax = dy;
    } else {
        dmin = dy;
        dmax = dx;
    }

    aptlval = 4 * dmin - 2 * dmax;
    startx = (x1 & 0xf) << 12;  // x1 modulo 16
    texture = ((x1 + pattern_offset) & 0xf) << 12;  // BSH in BLTCON1
    sign = (aptlval < 0 ? 1 : 0) << 6;
    bltcon1val = texture | sign | (code << 2) | (single << 1) | 0x01;

    start_address = canvas +
        plane * screen.planeSize +
        y1 * bytes_per_line + (x1>>3);


    WaitBlit();
    custom.bltapt = (APTR) ((UWORD) aptlval);
    custom.bltcpt = start_address;

    // this is actually only used for the first pixel of the line
    // if we point this to another memory area, the first pixel will
    // not be plotted. Research this more and then see whether we should
    // use that in the tutorial
    custom.bltdpt = omit_first_pixel ? scratchmem : start_address;

    custom.bltamod = 4 * (dmin - dmax);
    custom.bltbmod = 4 * dmin;

    custom.bltcmod = screen.width / 8;  // destination width in bytes
    custom.bltdmod = screen.width / 8;
    custom.bltcon0 = 0x0b00 | lf_byte | startx;
    custom.bltcon1 = bltcon1val;

    custom.bltadat = 0x8000;  // draw "pen" pixel
    custom.bltbdat = line_pattern;
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    custom.bltsize = ((dmax + 1) << 6) + 2;    
    
}

void screen_drawLine_on_plane_with_checks(USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE plane){
    if (x1<0) x1 = 0;
    if (x2<0) x2 = 0;
    if (y1<0) y1 = 0;
    if (y2<0) y2 = 0;
    if (x1>=screen.width) x1 = screen.width-1;
    if (x2>=screen.width) x2 = screen.width-1;
    if (y1>=screen.height) y1 = screen.height-1;
    if (y2>=screen.height) y2 = screen.height-1;
    screen_drawLine_on_plane(x1, y1, x2, y2, plane);
}


void screen_drawTriangle(USHORT x1, USHORT y1,USHORT x2, USHORT y2, USHORT x3, USHORT y3, UBYTE filled, UBYTE plane){
    USHORT minx;
    USHORT maxx;
    USHORT miny;
    USHORT maxy;

    if (x1<0) x1 = 0;
    if (x2<0) x2 = 0;
    if (x3<0) x3 = 0;
    minx = x1;
    maxx = x1;
    miny = y1;
    maxy = y1;


    screen_drawLine_on_plane(x1, y1, x2, y2, plane);
    screen_drawLine_on_plane(x2, y2, x3, y3, plane);
    screen_drawLine_on_plane(x3, y3, x1, y1, plane);

    if (filled) {
        // bounding box
        if (x2 < minx) minx = x2;
        if (x3 < minx) minx = x3;
        if (x2 > maxx) maxx = x2;
        if (x3 > maxx) maxx = x3;

        if (y2 < miny) miny = y2;
        if (y3 < miny) miny = y3;
        if (y2 > maxy) maxy = y2;
        if (y3 > maxy) maxy = y3;

        bitplane_fillArea_simple(plane, minx, miny+1, maxx, maxy-1, 0, 0);
    }
}


void screen_drawAsset_simple(BYTE index, UWORD x, UWORD y){
    struct ASSET asset = *getAsset(index);
    UBYTE *dst = screen_getCanvas();
    UBYTE *src = asset.data;
    USHORT bltsize = (screen.height << 6) | (screen.width >> 4);
    UBYTE planeCount = asset.planeCount;
    UBYTE i;
    
    WaitBlit();

    custom.bltcon0 = 0x09f0;     // enable channels A and D, LF => D = A
    custom.bltcon1 = 0;

    
    custom.bltafwm = 0xffff; // first word mask
    custom.bltalwm = 0xffff; // last word mask

    custom.bltamod = 0; //srcmod;
    custom.bltdmod = 0; // dstmod;

    custom.bltsize = bltsize;


    for (i = 0; i < planeCount; i++) {

        custom.bltapt = src; 
        custom.bltdpt = dst;
        custom.bltsize = bltsize;

        // Increase the pointers to the next plane
        src += asset.planeSize;
        dst += screen.planeSize;

        WaitBlit();
    }
    
}



// copy 1 bitplane from an asset to the screen
// the asset is assumed to be of the same width as the screen
// and at least as high as the screen

// good explanation: https://bumbershootsoft.wordpress.com/2024/07/27/amiga-500-using-our-full-power/

void screen_drawAssetPlane_simple(UBYTE index, UBYTE plane){

    UBYTE *src = getAssetData(index);
    UBYTE *dst = screen_getCanvas() + plane * screen.planeSize;
    USHORT bltsize = (screen.height << 6) | (screen.width >> 4);
    
    WaitBlit();

    custom.bltcon0 = 0x09f0;     // enable channels A and D, LF => D = A
    custom.bltcon1 = 0;

    custom.bltapt = src; 
    custom.bltafwm = 0xffff; // first word mask
    custom.bltalwm = 0xffff; // last word mask

    custom.bltdpt = dst;
    custom.bltamod = 0; //srcmod;
    custom.bltdmod = 0; // dstmod;

    custom.bltsize = bltsize;

    WaitBlit();
}

// copy a bitplane from an asset to the screen
// with a vertical offset and a height
// the asset is assumed to be of the same width as the screen
// and at least as high as the screen
// Note: the HEIGHT of the asset is not checked for speed reasons, 
// so make sure to not exceed this height when using negative Y
void screen_drawAssetPlane_y(UBYTE index, UBYTE plane, WORD y, USHORT height){

    UBYTE *src = getAssetData(index);
    UBYTE *dst = screen_getCanvas() + plane * screen.planeSize;
    USHORT bltsize;
    USHORT rowBytes = screen.width >> 3;

    if (y < 0) {
        src -= y * rowBytes;
        y = 0;
    }
    dst += y * rowBytes;

    if (y + height > screen.height) height = screen.height - y;
    if (height > screen.height) height = screen.height;
 
    bltsize = (height << 6) | screen.width >> 4;   
    
    WaitBlit();

    custom.bltcon0 = 0x09f0;     // enable channels A and D, LF => D = A
    custom.bltcon1 = 0;

    custom.bltapt = src; 
    custom.bltafwm = 0xffff; // first word mask
    custom.bltalwm = 0xffff; // last word mask

    custom.bltdpt = dst;
    custom.bltamod = 0; //srcmod;
    custom.bltdmod = 0; // dstmod;

    custom.bltsize = bltsize;

    WaitBlit();

}

void screen_drawAssetPlane(UBYTE index, UBYTE dstPlane, WORD x, WORD y, USHORT width, USHORT height, UBYTE srcPlane, UWORD srcX, UWORD srcY){
    struct ASSET asset = *getAsset(index);
    UBYTE *data = getAssetData(index);
    
    USHORT bX = 0;
    USHORT bY = 0;
    UBYTE bit = 0;
    UBYTE byte = 0;


    //int tile_width_pixels;
    int tile_x0;
    int blit_width;
    int blit_width0_pixels;
    int src_blit_width;
    int dst_x0;
    int dst_shift;
    int dst_blit_width;
    int dst_offset;
    int final_blit_width;
    USHORT srcmod;
    USHORT dstmod;
    USHORT bltsize;
    
    int bg_plane_size;
    char *src;
    char *mask;
    char *dst;
    int i;
    USHORT alwm;

    int dstx = x; 
    int dsty = y; 

    tile_x0 = 0;
    blit_width = width>>4;

     // width not a multiple of 16 ? -> add 1 to the width
    if (asset.width & 0x0f) blit_width++;

    blit_width0_pixels = blit_width * 16;
    
    src_blit_width = blit_width;
    if (tile_x0 > blit_width0_pixels - asset.width) src_blit_width++;

    // 2. Determine the amount of shift and the first word in the destination
    dst_x0 = dstx & 0x0f;  // destination x relative to the containing word
    dst_shift = dst_x0 - tile_x0;  // shift amount
    dst_blit_width = blit_width;
    dst_offset = 0;

    // negative shift => shift is to the left, so we extend the shift to the
    // left and right-shift in the previous word so we always right-shift
    if (dst_shift < 0) {
        dst_shift = 16 + dst_shift;
        dst_blit_width++;
        dst_offset = -2;
    }

    // make the blit wider if it needs more space
    if (dst_x0 > blit_width0_pixels - asset.width) {
        dst_blit_width++;
    }

    alwm = 0xffff;
    final_blit_width = src_blit_width;

    // due to relative positioning and shifts, the destination blit width
    // can be larger than the source blit, so we use the larger of the 2
    // and mask out last word of the source
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }

    WaitBlit();

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    //custom.bltcon0 = 0x0fca | (dst_shift << 12);
    
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);     // enable channels A and D, LF => D = A
    custom.bltcon1 = dst_shift << 12;

    srcmod = (asset.width>>3) - (final_blit_width<<1);
    dstmod = (screen.width>>3) - (final_blit_width<<1);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    bltsize = (height << 6) | (final_blit_width & 0x3f);

    bg_plane_size = screen.width / 8 * screen.height;

    src = data + srcY * asset.width / 8 + srcX / 8;
    // The mask data is the plane after the source image planes
    // mask = data + srcy * 16 / 8 + srcx / 8;
    dst = screen_getCanvas() + dsty * screen.width / 8 + dstx / 8 + dst_offset;

    src += srcPlane * asset.planeSize;
    dst += dstPlane * screen.planeSize;

    //for (i = 0; i < asset.planeCount; i++) {

        custom.bltapt = src; 
        custom.bltbpt = src;
        custom.bltcpt = dst;
        custom.bltdpt = dst;
        custom.bltsize = bltsize;

        // Increase the pointers to the next plane
        //src += asset.planeSize;
        //dst += screen.planeSize;

        WaitBlit();
    //}
}



void screen_drawAsset(UBYTE index, WORD x, WORD y, USHORT width, USHORT height, UWORD srcX, UWORD srcY){
    struct ASSET asset = *getAsset(index);
    UBYTE *data = getAssetData(index);
    
    USHORT imageWidth = screen.width;
    USHORT imageHeight = screen.height;
    USHORT bX = 0;
    USHORT bY = 0;
    UBYTE bit = 0;
    UBYTE byte = 0;


    int tile_width_pixels;
    int tile_x0;
    int blit_width;
    int blit_width0_pixels;
    int src_blit_width;
    int dst_x0;
    int dst_shift;
    int dst_blit_width;
    int dst_offset;
    int final_blit_width;
    USHORT srcmod;
    USHORT dstmod;
    USHORT bltsize;
    
    int bg_plane_size;
    char *src;
    char *mask;
    char *dst;
    int i;
    USHORT alwm;

    int dstx = x; 
    int dsty = y; 

    tile_width_pixels = width;
    //printf("Tile width: %d\n",tile_width_pixels);
    tile_x0 = 0;
    blit_width = tile_width_pixels / 16;

     // width not a multiple of 16 ? -> add 1 to the width
    if (tile_width_pixels & 0x0f) blit_width++;

    blit_width0_pixels = blit_width * 16;
    
    src_blit_width = blit_width;
    if (tile_x0 > blit_width0_pixels - tile_width_pixels) src_blit_width++;

    // 2. Determine the amount of shift and the first word in the destination
    dst_x0 = dstx & 0x0f;  // destination x relative to the containing word
    dst_shift = dst_x0 - tile_x0;  // shift amount
    dst_blit_width = blit_width;
    dst_offset = 0;

    // negative shift => shift is to the left, so we extend the shift to the
    // left and right-shift in the previous word so we always right-shift
    if (dst_shift < 0) {
        dst_shift = 16 + dst_shift;
        dst_blit_width++;
        dst_offset = -2;
    }

    // make the blit wider if it needs more space
    if (dst_x0 > blit_width0_pixels - tile_width_pixels) {
        dst_blit_width++;
    }

    alwm = 0xffff;
    final_blit_width = src_blit_width;

    // due to relative positioning and shifts, the destination blit width
    // can be larger than the source blit, so we use the larger of the 2
    // and mask out last word of the source
    if (dst_blit_width > src_blit_width) {
        final_blit_width = dst_blit_width;
        alwm = 0;
    }

    WaitBlit();

    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    //custom.bltcon0 = 0x0fca | (dst_shift << 12);
    
    custom.bltcon0 = 0x09f0 | (dst_shift << 12);     // enable channels A and D, LF => D = A
    
    custom.bltcon1 = dst_shift << 12;

    srcmod = asset.width / 8 - (final_blit_width * 2);
    dstmod = screen.width / 8 - (final_blit_width * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    bltsize = (height << 6) | (final_blit_width & 0x3f);

 


    bg_plane_size = screen.width / 8 * screen.height;

    src = data + srcY * asset.width / 8 + srcX / 8;
    // The mask data is the plane after the source image planes
    // mask = data + srcy * 16 / 8 + srcx / 8;
    dst = screen_getCanvas() + dsty * screen.width / 8 + dstx / 8 + dst_offset;

    for (i = 0; i < asset.planeCount; i++) {

        custom.bltapt = src; 
        custom.bltbpt = src;
        custom.bltcpt = dst;
        custom.bltdpt = dst;
        custom.bltsize = bltsize;

        // Increase the pointers to the next plane
        src += asset.planeSize;
        dst += screen.planeSize;

        WaitBlit();
    }

}

void screen_overrideBitPlane(UBYTE plane, APTR address){
    unsigned long addr = address;
    overideBitPlanes[plane] = address;

    if (!screen.doubleBuffer){
        if (addr == 0){
            addr = (unsigned long) (screen.canvas + (plane * screen.planeSize));
        }
        copper_setBitPlane(plane*4,(addr >> 16) & 0xffff);
        copper_setBitPlane(plane*4 + 2,addr & 0xffff);
    }
}

struct DScreen *screen_getScreen(void){
    return &screen;
}

// returns a pointer the screen canvas that is currently being drawn to
// (when double buffering is enabled, this is the back buffer)
UBYTE *screen_getCanvas(void){
    if (screen.doubleBuffer){
        return screen.activeScreen == 0 ? screen.canvas2 : screen.canvas;
    }
    return screen.canvas;
}