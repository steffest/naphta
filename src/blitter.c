#include <hardware/custom.h>
#include "app.h"
extern struct Custom far custom;
#define SHIFT_PADDING (16)
#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))


void blit_asset(UBYTE index, int dstx, int dsty){

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
    unsigned short srcmod;
    unsigned short dstmod;
    unsigned short bltsize;
    int srcx;
    int srcy;
    int bobs_plane_size;
    int bg_plane_size;
    char *src;
    char *mask;
    char *dst;
    int i;
    unsigned short alwm;
    int tilex = 0;
    int tiley = 0;

    struct ASSET asset = *getAsset(index);

    // actual object width (without the padding)
    tile_width_pixels = asset.width - SHIFT_PADDING;

    // this tile's x-position relative to the word containing it
    tile_x0 = asset.width * tilex & 0x0f;


    // 1. determine how wide the blit actually is
    blit_width = tile_width_pixels / 16;


    // width not a multiple of 16 ? -> add 1 to the width
    if (tile_width_pixels & 0x0f) blit_width++;

    blit_width0_pixels = blit_width * 16;  // blit width in pixels

    // Final source blit width: does the tile extend into an additional word ?
    src_blit_width = blit_width;
    if (tile_x0 > blit_width0_pixels - tile_width_pixels) src_blit_width++;

    // 2. Determine the amount of shift and the first word in the
    // destination
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

    // set fillMode
    custom.bltafwm = 0xffff;
    custom.bltalwm = alwm;

    // cookie cut enable channels B, C and D, LF => D = AB + ~AC => 0xca
    // A = Mask sheet
    // B = Tile sheet
    // C = Background
    // D = Background
    custom.bltcon0 = 0x0fca | (dst_shift << 12);
    custom.bltcon1 = dst_shift << 12;  // shift in B

    // modulos are in bytes
    srcmod = asset.width / 8 - (final_blit_width * 2);
    dstmod = SCREENWIDTH / 8 - (final_blit_width * 2);
    custom.bltamod = srcmod;
    custom.bltbmod = srcmod;
    custom.bltcmod = dstmod;
    custom.bltdmod = dstmod;

    // The blit size is the size of a plane of the tile size (1 word * 16)
    bltsize = ((asset.height) << 6) |
        (final_blit_width & 0x3f);

    // map the tile position to physical coordinates in the tile sheet
    srcx = tilex * asset.width;
    srcy = tiley * asset.height;

    bobs_plane_size = asset.width / 8 * asset.height;
    bg_plane_size = SCREENWIDTH / 8 * SCREENHEIGHT;

    src = asset.data + srcy * asset.width / 8 + srcx / 8;
    // The mask data is the plane after the source image planes
    mask = 0;
    dst = screen_getCanvas() + dsty * SCREENWIDTH / 8 +
        dstx / 8 + dst_offset;   

    for (i = 0; i < asset.planeCount; i++) {

        custom.bltapt = src;
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

void blitter_fillRect(UBYTE *target, int x, int y, int width, int height, UBYTE fill){
    
  APTR dst_base = target;
  UWORD dst_stride_b = SCREENWIDTH >> 3;
  APTR mask_base = 0;
  UWORD mask_x = 0;
  UWORD mask_y = 0;
  UWORD mask_stride_b = 0;
  UWORD start_x_word = x >> 4;
  UWORD end_x_word = ((x + width) + 0xF) >> 4;
  UWORD width_words = end_x_word - start_x_word;
  UWORD word_offset = x & 0xF;

  UWORD dst_mod_b = dst_stride_b - (width_words * kBytesPerWord);
  UWORD mask_mod_b = mask_stride_b - (width_words * kBytesPerWord);

  ULONG dst_start_b = (ULONG)dst_base + (y * dst_stride_b) + (start_x_word * kBytesPerWord);
  ULONG mask_start_b = (ULONG)mask_base + (mask_y * mask_stride_b) + (start_x_word * kBytesPerWord);

  UWORD left_word_mask = (UWORD)(0xFFFFU << (word_offset + MAX(0, 0x10 - (word_offset + width)))) >> word_offset;
  UWORD right_word_mask;

  UWORD minterm = 0xA;

  if (width_words == 1) {
    right_word_mask = left_word_mask;
  }
  else {
    right_word_mask = 0xFFFFU << MIN(0x10, ((start_x_word + width_words) << 4) - (x + width));
  }

  if (mask_base) {
    minterm |= fill ? 0xB0 : 0x80;
  } else {
    minterm |= fill ? 0xF0 : 0x00;
  }

  WaitBlit();
  //gfx_wait_blit();

  // A = Mask of bits inside copy region
  // B = Optional bitplane mask
  // C = Destination data (for region outside mask)
  // D = Destination data
  custom.bltcon0 = BLTCON0_USEC | BLTCON0_USED | (mask_base ? BLTCON0_USEB : 0) | minterm;
  custom.bltcon1 = 0;
  custom.bltbmod = mask_mod_b;
  custom.bltcmod = dst_mod_b;
  custom.bltdmod = dst_mod_b;
  custom.bltafwm = left_word_mask;
  custom.bltalwm = right_word_mask;
  custom.bltadat = 0xFFFF;
  custom.bltbpt = (APTR)mask_start_b;
  custom.bltcpt = (APTR)dst_start_b;
  custom.bltdpt = (APTR)dst_start_b;
  custom.bltsize = (height << BLTSIZE_H0_SHF) | width_words;

}