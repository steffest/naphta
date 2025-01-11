#include <exec/memory.h>
#include <hardware/custom.h>
#include "app.h"

#define LF_XOR 0x4a
#define LF_OR 0x46

#define MIN(X, Y) ((X) < (Y) ? (X) : (Y))
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))

extern struct Custom far custom;

void bitplane_clear(UBYTE plane){
    // I think this is the fastest way to clear a bitplane?
    // it is at least the most simple blit I coulmd come up with
    // (used for clearing a consecutive area of memory where the bitplane is stored)
    WaitBlit();
    custom.bltcon0 = BLTCON0_USED; // only D channel; 0X0100
    custom.bltcon1 = 0;
    custom.bltdmod = 0; // ^^destination modulo: 0 because we are clearing the whole width
    custom.bltdpt = screen_getCanvas() + (plane * PLANESIZE); // destination pointer
    custom.bltsize = (SCREENHEIGHT << 6) | (SCREENWIDTH >> 4);
    WaitBlit();
}

void bitplane_clearRect(UBYTE plane, UWORD x, UWORD y, UWORD w, UWORD h) {
    UWORD x2 = x + w;
    UWORD y2 = y + h;
    bitplane_fillRect(plane, x, y, x2, y2, 0);
};

void bitplane_drawRect(UBYTE plane, UWORD x, UWORD y, UWORD w, UWORD h, BOOL fill) {
    UWORD x2 = x + w;
    UWORD y2 = y + h;

    if (fill){
        //bitplane_fillArea(plane, x, y, x2, y2, 0, 1);
        bitplane_fillRect(plane, x, y, w+1, h+1, 1);
    }else{
        bitplane_drawLine(plane, x, y, x, y2,1);
        bitplane_drawLine(plane, x2, y, x2, y2,1);
        x++;
        x2--;
        bitplane_drawLine(plane, x, y, x2, y,0);
        bitplane_drawLine(plane, x, y2, x2, y2,0);
    }
};

void bitplane_fillRect(UBYTE plane, USHORT dst_x, USHORT dst_y, USHORT width, USHORT height, UBYTE set_bits){
  UBYTE *canvas = screen_getCanvas();
  APTR dst_base = canvas + (plane * PLANESIZE);
  UWORD dst_stride_b = SCREENWIDTH >> 3;
  APTR mask_base = 0;
  UWORD mask_x = 0;
  UWORD mask_y = 0;
  UWORD mask_stride_b = 0;
  UWORD start_x_word = dst_x >> 4;
  UWORD end_x_word = ((dst_x + width) + 0xF) >> 4;
  UWORD width_words = end_x_word - start_x_word;
  UWORD word_offset = dst_x & 0xF;

  UWORD dst_mod_b = dst_stride_b - (width_words * kBytesPerWord);
  UWORD mask_mod_b = mask_stride_b - (width_words * kBytesPerWord);

  ULONG dst_start_b = (ULONG)dst_base + (dst_y * dst_stride_b) + (start_x_word * kBytesPerWord);
  ULONG mask_start_b = (ULONG)mask_base + (mask_y * mask_stride_b) + (start_x_word * kBytesPerWord);

  UWORD left_word_mask = (UWORD)(0xFFFFU << (word_offset + MAX(0, 0x10 - (word_offset + width)))) >> word_offset;
  UWORD right_word_mask;

  UWORD minterm = 0xA;

  if (width_words == 1) {
    right_word_mask = left_word_mask;
  }
  else {
    right_word_mask = 0xFFFFU << MIN(0x10, ((start_x_word + width_words) << 4) - (dst_x + width));
  }

  if (mask_base) {
    minterm |= set_bits ? 0xB0 : 0x80;
  }
  else {
    minterm |= set_bits ? 0xF0 : 0x00;
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


// no checks for the area to be word aligned
void bitplane_fillArea_simple(UBYTE plane, USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE exclusive, UBYTE fill_carry_input){
    
    UBYTE *canvas = screen_getCanvas();

    int width = x2 - x1;
    int height = y2 - y1;
    int num_words = width / 16;
    UBYTE fill_mode = exclusive ? 16 : 8;
    USHORT bltmod;
    UBYTE *src;

    WaitBlit();
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    custom.bltcon0 = 0x09f0;       // enable channels A and D, LF => D = A
    // descending mode + fill parameters
    custom.bltcon1 = fill_mode | (fill_carry_input << 2) | 0x2;

    // modulos are in bytes
    bltmod = (SCREENWIDTH - width) / 8;
    src = canvas + (y2 * SCREENWIDTH / 8) + x2 / 8 - 2 + plane * PLANESIZE;

    custom.bltdpt = src;
    custom.bltapt = src;
    custom.bltdmod = bltmod;
    custom.bltamod = bltmod;

    custom.bltsize = (height << 6) | (num_words & 0x3f);
    WaitBlit();
}

void bitplane_fillArea(UBYTE plane, USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE exclusive, UBYTE fill_carry_input){
    
    UBYTE *canvas = screen_getCanvas();
    
    // determine the left and right borders, which are at the
    // word boundaries to the left and right sides
    int left = x1 - (x1 & 0x0f);
    int right = x2 + 16 - (x2 & 0x0f);
    int blit_width_pixels = right - left;
    int blit_height = (y2 - y1) + 1;
    int num_words = blit_width_pixels / 16;
    UBYTE fill_mode = exclusive ? 16 : 8;
    USHORT bltmod;
    UBYTE *src;

    WaitBlit();
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    custom.bltcon0 = 0x09f0;       // enable channels A and D, LF => D = A
    // descending mode + fill parameters
    custom.bltcon1 = fill_mode | (fill_carry_input << 2) | 0x2;

    // modulos are in bytes
    bltmod = (SCREENWIDTH - blit_width_pixels) / 8;
    // the address of source A and D has to be the word that defines the right
    // bottom corner
    src = canvas + (y2 * SCREENWIDTH / 8) +
	right / 8 - 2 + plane * (SCREENWIDTH / 8 * SCREENHEIGHT);

    custom.bltdpt = src;
    custom.bltapt = src;
    custom.bltdmod = bltmod;
    custom.bltamod = bltmod;

    custom.bltsize = (blit_height << 6) | (num_words & 0x3f);
    WaitBlit();
}

void bitplane_drawLine(UBYTE plane, UWORD x1, UWORD y1, UWORD x2, UWORD y2, UBYTE singlePixelPerLine) {
    USHORT dx = abs(x2 - x1);
    USHORT dy = abs(y2 - y1);
    USHORT dmin, dmax;
    USHORT bytes_per_line = SCREENWIDTH >> 3;
    UBYTE code;
    short aptlval;
    USHORT startx;
    USHORT texture;
    USHORT sign;
    USHORT bltcon1val;

    USHORT line_pattern = 0xffff;
    UBYTE pattern_offset = 0;
    UBYTE lf_byte = 0x4a;
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
    bltcon1val = texture | sign | (code << 2) | (singlePixelPerLine << 1) | 0x01;

    start_address = canvas +
        plane * PLANESIZE +
        y1 * bytes_per_line + (x1>>3);


    WaitBlit();
    custom.bltapt = (APTR) ((UWORD) aptlval);
    custom.bltcpt = start_address;
    custom.bltdpt = start_address;

    custom.bltamod = 4 * (dmin - dmax);
    custom.bltbmod = 4 * dmin;

    custom.bltcmod = SCREENWIDTH / 8;  
    custom.bltdmod = SCREENWIDTH / 8;
    custom.bltcon0 = 0x0b00 | lf_byte | startx;
    custom.bltcon1 = bltcon1val;

    custom.bltadat = 0x8000;  
    custom.bltbdat = 0xffff; // line pattern
    custom.bltafwm = 0xffff;
    custom.bltalwm = 0xffff;

    custom.bltsize = ((dmax + 1) << 6) + 2;    

};