#include <stdio.h>
#include <hardware/custom.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <graphics/gfxbase.h>
#include "app.h"


#define PLAYER_APPEAR 200
#define PLAYER_FADEBACKGROUND (PLAYER_APPEAR+320)
#define PLAYER_TOSPRITE (PLAYER_FADEBACKGROUND+50)
#define PLAYER_TOSPRITE2 (PLAYER_TOSPRITE+1)
#define PLAYER_TOSPRITE3 (PLAYER_TOSPRITE+2)
#define PLAYER_TOSPRITE4 (PLAYER_TOSPRITE+3)
#define PLAYER_TURN (PLAYER_TOSPRITE4+1)
#define PLAYER_UP (PLAYER_TURN+50)
#define PLAYER_SWING (PLAYER_UP+150)
#define PLAYER_SWING2 (PLAYER_SWING+70)
#define STROBO (PLAYER_SWING2+50)

extern struct GfxBase *GfxBase;
extern struct Custom far custom;

BYTE backgroundAsset;
BYTE fogAsset;
BYTE trumpetAsset;


// sprite strucure: first 2 words (VStart,HStart,VStop) will be set by the sprite_setPosition function;
// the rest of the sprite data is the actual image data
// where each word is a row of bits of the 16 pixels of the plane, followed by the same row of bits for the next plane

static UWORD __chip trumpet_sprite8[] = {
 0x0000, 0x0000, 
  0x0000, 0x0001, 
  0x0001, 0x0000, 
  0x0001, 0x0002, 
  0x0007, 0x0000, 
  0x002f, 0x0030, 
  0x00bf, 0x00c0, 
  0x00ff, 0x0100, 
  0x01ff, 0x0200, 
  0x0bff, 0x0c00, 
  0x17ff, 0x1800, 
  0x0fff, 0x1000, 
  0x3fff, 0x2000, 
  0x1fff, 0x2000, 
  0x3fff, 0x0000, 
  0x7fff, 0x4000, 
  0x7fff, 0x4000, 
  0x3fff, 0x4000, 
  0x7fff, 0x0000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0xffff, 0x8000, 
  0x3fff, 0x4000, 
  0x7fff, 0x4000, 
  0x3fff, 0x0000, 
  0x1fff, 0x2000, 
  0x1fff, 0x2000, 
  0x1fff, 0x2000, 
  0x1fff, 0x0000, 
  0x1fff, 0x0000, 
  0x0fff, 0x1000, 
  0x0fff, 0x1000, 
  0x0fff, 0x0000, 
  0x07ff, 0x0800, 
  0x07ff, 0x0800, 
  0x07ff, 0x0800, 
  0x07ff, 0x0800, 
  0x07ff, 0x0000, 
  0x03ff, 0x0400, 
  0x03ff, 0x0400, 
  0x01ff, 0x0200, 
  0x01ff, 0x0200, 
  0x01ff, 0x0000, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0100, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x07ff, 0x0000, 
  0x07ff, 0x0000, 
  0x07ff, 0x0000, 
  0x07ff, 0x0000, 
  0x07ff, 0x0000, 
  0x07ff, 0x0000, 
  0x07ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x01ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x00ff, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x007f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0000, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite7[] = {
  0x0000, 0x0000, 
  0x0001, 0x0000, 
  0x0007, 0x0004, 
  0x000f, 0x0008, 
  0x001f, 0x0010, 
  0x001f, 0x0000, 
  0x003f, 0x0000, 
  0x003f, 0x0040, 
  0x00ff, 0x0080, 
  0x00ff, 0x0000, 
  0x01ff, 0x0100, 
  0x00ff, 0x0100, 
  0x01ff, 0x0000, 
  0x01ff, 0x0200, 
  0x01ff, 0x0200, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x01ff, 0x0200, 
  0x01ff, 0x0200, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x03ff, 0x0000, 
  0x07ff, 0x0400, 
  0x03ff, 0x0400, 
  0x07ff, 0x0000, 
  0x0fff, 0x0800, 
  0x0fff, 0x0000, 
  0x2fff, 0x7000, 
  0x7fff, 0x8000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite6[] = {
  0x0000, 0x0000, 
  0x4e50, 0x71b0, 
  0xfff4, 0x000c, 
  0xfff3, 0x000f, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xfffc, 0x0002, 
  0xfffe, 0x0002, 
  0xfffc, 0x0000, 
  0xfff8, 0x0004, 
  0xfcf0, 0x0308, 
  0xfc00, 0x0200, 
  0xff60, 0x00e0, 
  0xffe0, 0x0010, 
  0xfffc, 0x0004, 
  0xfffe, 0x0002, 
  0xfffe, 0x0001, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite5[] = {
  0x0000, 0x0000, 
  0xc000, 0x4000, 
  0xc000, 0x2000, 
  0xf000, 0x0000, 
  0xf000, 0x0800, 
  0xf800, 0x0400, 
  0xff80, 0x0080, 
  0xffc0, 0x0040, 
  0xffc0, 0x0040, 
  0xff00, 0x0080, 
  0xffe0, 0x0020, 
  0xffc0, 0x0020, 
  0xffc0, 0x0020, 
  0xffe0, 0x0000, 
  0xffe0, 0x0020, 
  0xff20, 0x00e0, 
  0xff80, 0x0380, 
  0xf800, 0x0400, 
  0xfc00, 0x0400, 
  0xf800, 0x0000, 
  0xf800, 0x0000, 
  0xf000, 0x0800, 
  0xf800, 0x0000, 
  0xe000, 0x1000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0x8000, 0x6000, 
  0xc000, 0x4000, 
  0xd007, 0x3007, 
  0xfd8f, 0x0390, 
  0x17fd, 0x1802, 
  0x007f, 0x0080, 
  0x00e2, 0x0023, 
  0x0081, 0x0141, 
  0x01c1, 0x0041, 
  0x0381, 0x0201, 
  0x0300, 0x0280, 
  0x0301, 0x0281, 
  0x0180, 0x0001, 
  0x01c1, 0x0040, 
  0x01c3, 0x0102, 
  0x80c3, 0x4022, 
  0xc079, 0x2006, 
  0xe01f, 0x1020, 
  0xf80f, 0x080c, 
  0xfc03, 0x0404, 
  0xfc07, 0x0008, 
  0xfe3f, 0x0220, 
  0xfe7f, 0x0040, 
  0xff7f, 0x0100, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xfe7f, 0x0180, 
  0xfe3f, 0x0140, 
  0xfe1f, 0x0100, 
  0xff07, 0x0008, 
  0xff03, 0x0004, 
  0xff81, 0x0002, 
  0xff80, 0x0000, 
  0xff80, 0x0000, 
  0xff80, 0x0000, 
  0xff80, 0x0000, 
  0xff80, 0x0000, 
  0xffc0, 0x0000, 
  0xffc0, 0x0000, 
  0xffc0, 0x0000, 
  0xffe0, 0x0000, 
  0xffe0, 0x0000, 
  0xffe0, 0x0000, 
  0xffe0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff0, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfff8, 0x0000, 
  0xfffc, 0x0000, 
  0xfffc, 0x0000, 
  0xfffc, 0x0000, 
  0xfffc, 0x0000, 
  0xfffc, 0x0000, 
  0xfffe, 0x0000, 
  0xfffe, 0x0000, 
  0xfffe, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite4[] = {
  0x0000, 0x0000, 
  0x0000, 0x0080, 
  0x01cc, 0x0008, 
  0x01ce, 0x0250, 
  0xd3bc, 0xe000, 
  0xf3bd, 0x0c44, 
  0xffff, 0x0004, 
  0xbfff, 0x4000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xbfff, 0x4000, 
  0xffff, 0x0000, 
  0x7fff, 0x8000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xfeff, 0x0100, 
  0xffff, 0x0200, 
  0xffff, 0x0000, 
  0xe7ff, 0x1400, 
  0xe3ff, 0x0200, 
  0xe3ff, 0x1001, 
  0xffff, 0x0c01, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xefff, 0x1800, 
  0xcfff, 0x2800, 
  0xc7ff, 0x0800, 
  0x8fff, 0x4000, 
  0xdfff, 0x3000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xfffe, 0x0001, 
  0xfffe, 0x0001, 
  0xfffe, 0x0001, 
  0xffff, 0x0001, 
  0xffff, 0x0001, 
  0xfffe, 0x0000, 
  0xfffc, 0x0002, 
  0xfffe, 0x0002, 
  0xfffe, 0x0002, 
  0xfff8, 0x0004, 
  0x7ffc, 0x8004, 
  0x3ff8, 0x4000, 
  0x1ff0, 0x2008, 
  0x07f0, 0x0800, 
  0x00e0, 0x0310, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite3[] = {
  0x0000, 0x0000, 
  0xc000, 0x0000, 
  0xc000, 0x2000, 
  0x8000, 0x6000, 
  0xd380, 0x2f80, 
  0xffe3, 0x001f, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xf97f, 0x0580, 
  0xf4cd, 0x0cce, 
  0xe000, 0x1800, 
  0xf000, 0x1000, 
  0xfa01, 0x0601, 
  0xffd1, 0x0031, 
  0xff7c, 0x0083, 
  0xff1f, 0x00e0, 
  0xf9ff, 0x3e00, 
  0xe7ff, 0x1fc0, 
  0xfe7d, 0x01ce, 
  0xfc77, 0x038e, 
  0xb7bf, 0xb840, 
  0x01ff, 0x0100, 
  0x004b, 0x007c, 
  0x0000, 0x0000, 
  0x8000, 0x0000, 
  0xc000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xe000, 0x0000, 
  0xc000, 0x0000, 
  0x8000, 0x0000, 
  0x8000, 0x0000, 
  0x8000, 0x0000, 
  0x8000, 0x0000, 
  0x8000, 0x0000, 
  0x8000, 0x0000, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite2[] = {
  0x0000, 0x0000, 
  0x8000, 0x8000, 
  0xe1ff, 0x1fff, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xffff, 0x0000, 
  0xbf97, 0x6098, 
  0x0303, 0x8482, 
  0x03c0, 0x8240, 
  0x0080, 0x8140, 
  0x01c0, 0x8100, 
  0xf1c0, 0x1100, 
  0xf9c0, 0x0900, 
  0xf080, 0x0940, 
  0xf9e0, 0x0020, 
  0xfbc0, 0x0220, 
  0xffc0, 0x0000, 
  0xffc0, 0x0040, 
  0xcf00, 0x3080, 
  0xcf00, 0xc900, 
  0x0800, 0x0c00, 
  0x0000, 0x0000
};
static UWORD __chip trumpet_sprite1[] = {
  0x0000, 0x0000, 
  0x0080, 0x00c0, 
  0x01e0, 0x0000, 
  0x01f0, 0x0210, 
  0x07e0, 0x0410, 
  0x03e0, 0x0410, 
  0x0ff0, 0x0800, 
  0x0ff0, 0x1000, 
  0xbff0, 0xc000, 
  0xffe0, 0x0010, 
  0xffe0, 0x0010, 
  0xffe0, 0x0010, 
  0xffe0, 0x0010, 
  0xfff0, 0x0010, 
  0xffe0, 0x0000, 
  0xffc0, 0x0020, 
  0xffe0, 0x0020, 
  0xffe0, 0x8020, 
  0x7fc0, 0x4000, 
  0x1fc0, 0x2040, 
  0x1f80, 0x2000, 
  0x3f80, 0x2080, 
  0x3e00, 0x2100, 
  0x1e00, 0x0000, 
  0x1000, 0x1c00, 
  0x0000, 0x0000
};

static UBYTE trumpet_heights[] = {24, 23, 42, 63, 180, 183, 182, 147};
static UBYTE trumpet_offsetY[] = {6,0,4,7,35,38,37,2};
static BYTE trumpet_shiftX[] = {-44,-33,-22,-10,-4,0,2,4};
static BYTE trumpet_shiftY[] = {-11,-8,-6,-4,-2,0,2,4};

UWORD trumpet_x = 344;
UWORD trumpet_y = 160;

UWORD trumpet_shiftXPosition = 100;
BYTE trumpet_shiftYDirection = -1;

static UWORD palette1[] = {0x000,0x111,0x122,0x244,0x365,0x587,0x7a9,0xacb, // background
                            0X000,0X233,0X244,0X366,0X498,0X6ba,0X8dc,0Xcfe, // fog
                            0X223,0X233,0X234,0X357,0X589,0X7ab,0X9dd,0Xcff, // spotlight
                            0X233,0X445,0X467,0X58a,0X7bd,0X9df,0Xbff,0Xfff  // fog under sportlight
                            };

UWORD blendColorLight = 0x6c7;
UWORD blendColorDark = 0x156;
UBYTE blendAmount = 0;
UBYTE blendColorIndex = 0;
BYTE blendSpeed = 1;
UBYTE spotLightPlane = 4;

static UWORD blendColors[] = {
    0x6c7,0x156,
    0xd6d,0x614,
    0x8be,0x145,
};

UBYTE zIndex = 40;
BOOL fadeIn = TRUE;
UBYTE fadeOut = 0;
BOOL useFill = FALSE;

static UWORD * getSpriteData(UBYTE index) {
    // F me .... how do you setup an array of var sized arrays in C?
    switch (index) {
        case 0:
            return trumpet_sprite1;
        case 1:
            return trumpet_sprite2;
        case 2:
            return trumpet_sprite3;
        case 3:
            return trumpet_sprite4;
        case 4:
            return trumpet_sprite5;
        case 5:
            return trumpet_sprite6;
        case 6:
            return trumpet_sprite7;
        case 7:
            return trumpet_sprite8;
    }
    return trumpet_sprite1;
}

static UWORD getSpriteXOffset_swing(UBYTE index) {
    WORD max = trumpet_shiftX[index];
    WORD value;

    value = max * math_sin2(trumpet_shiftXPosition + 90) >> 8;
    
    return (UWORD) (trumpet_x - (index << 4) + value + (math_sin2(trumpet_shiftXPosition) >> 6));
}

static UWORD getSpriteYOffset_swing(UBYTE index) {
    WORD max = trumpet_shiftY[index] * trumpet_shiftYDirection;
    WORD value;

    value = max * math_sin2(trumpet_shiftXPosition + 75) >> 8;
    
    return (UWORD) (trumpet_y - trumpet_offsetY[index] + value);
}

static UWORD getSpriteXOffset_contracted(UBYTE index) {
    WORD max = trumpet_shiftX[index];
    return (UWORD) (trumpet_x - (index  << 4) + max);
}

static UWORD getSpriteXOffset_expanded(UBYTE index) {
    return (UWORD) (trumpet_x - (index  << 4));
}

static UWORD getSpriteXOffset_appear(UBYTE index) {
    WORD max = trumpet_shiftX[index];
    WORD value;
    value = (max * (128-trumpet_shiftXPosition)) >> 7;
    return (UWORD) (trumpet_x - (index  << 4) + value);
}

static UWORD getSpriteYOffset_appear(UBYTE index) {
    WORD max = trumpet_shiftY[index] * trumpet_shiftYDirection;
    WORD value;
    value = (max * (trumpet_shiftXPosition)) >> 7;
    return (UWORD) (trumpet_y - trumpet_offsetY[index] + value);
}

static UWORD getSpriteYOffset_up(UBYTE index) {
    WORD max = trumpet_shiftY[index] * trumpet_shiftYDirection;
    WORD value;
    value = (max * (128-trumpet_shiftXPosition)) >> 7;
    return (UWORD) (trumpet_y - trumpet_offsetY[index] + value);
}

static UWORD getSpriteYOffset_expanded(UBYTE index) {
    WORD max = trumpet_shiftY[index];
    return (UWORD) (trumpet_y - trumpet_offsetY[index] + max);
}

void setBackgroundBlend(UBYTE blend) {
    UBYTE i;
    for (i = 0; i < 32; i++) {
        palette_setBlendColor(i, 0x000, blend);
    }
}

void setSpotLightPalette(UWORD *palette) {
    UBYTE i;

    if (spotLightPlane == 3){
        
        for (i = 2; i < 8; i++) {
            palette_fluctuateColor(i, palette[i], -2, 2 , 7, 1);
            palette_setColorCycleIndex(i, i-1);
        }


        for (i = 9; i < 16; i++) {
            palette_fluctuateColor(i, palette[i], -3, 3 , 3, 1);
            palette_setColorCycleIndex(i, i-9);
        }
    }else{

        for (i = 3; i < 8; i++) {
            palette_fluctuateColor(i, palette[i], -1, 2 , 4, 1);
            //palette_setColorCycleIndex(i, i-1);
        }

        for (i = 17; i < 32; i++) {
            palette_fluctuateColor(i, palette[i], -1, 2 , 4, 1);
            //palette_setColorCycleIndex(i, 1);
        }

        if (fadeIn){
            for (i = 1; i < 32; i++) {
                palette_addColorCycle(i, palette[i]);
                palette_setBlendColor(i, 0x000, 255);
            }
        }

         
    }

}

void setSpotLightBlend(UBYTE blend) {
    UBYTE i;
    if (spotLightPlane == 3){
        for (i = 9; i < 12; i++) {
            palette_setBlendColor(i, blendColorDark, blend);
        }
        for (i = 12; i < 16; i++) {
            palette_setBlendColor(i, blendColorLight, blend);
        }
    }else{
        for (i = 17; i < 20; i++) {
            palette_setBlendColor(i, blendColorDark, blend);
        }
        for (i = 20; i < 24; i++) {
            palette_setBlendColor(i, blendColorLight, blend);
        }
        // fog under spotlight
        for (i = 24; i < 27; i++) {
            palette_setBlendColor(i, blendColorDark, blend);
        }
        for (i = 27; i < 32; i++) {
            palette_setBlendColor(i, blendColorLight, blend);
        }
    }
}

void activateSprites(void) {
    UBYTE i;

    // remove cycle colors of sprite colors
    for (i = 16; i < 32; i++) {
        palette_removeColorCycle(i);
    }

    // set sprite colors
    // sprite 0 and 1
    copper_setColorValue(17, 0x000);
    copper_setColorValue(18, 0x333);
    copper_setColorValue(19, 0x555);

    // sprite 2 and 3
    copper_setColorValue(21, 0x000);
    copper_setColorValue(22, 0x333);
    copper_setColorValue(23, 0x555);

    // sprite 3 and 4
    copper_setColorValue(25, 0x000);
    copper_setColorValue(26, 0x333);
    copper_setColorValue(27, 0x555);

     // sprite 6 and 7
    copper_setColorValue(29, 0x000);
    copper_setColorValue(30, 0x333);
    copper_setColorValue(31, 0x555);

}


UBYTE trumpetplayer_preload(void){
    loader_next();
    backgroundAsset = asset_loadImage("data/spotlight1.planes", 320, 256, 3, MEMF_ANY);
    loader_next();
    //fogAsset = asset_loadImage("data/fog1.planes", 320, 256, 1, MEMF_ANY);
    //loader_next();
    trumpetAsset = asset_loadImage("data/trumpetplayer_narrow.planes", 80, 180, 3, MEMF_ANY);
    loader_next();
    return 1;
}

void trumpetplayer(void){
    int i;
    int y;

    BOOL loop = TRUE;
    int currFrame = 0;
    int currAngle = 0;
    int stepFrame = 0;

    // fog
    BOOL fogActive = TRUE;
    WORD fogY = 0;

    // spotlights;
    BOOL spotlightActive = TRUE;
    BOOL spotlightFill = TRUE;
    BYTE xd;
    BYTE xd2;

    // trumpets
    UBYTE trumpetDrawCount = 0;
    UBYTE trumpetDrawSegment = 0;
    BYTE trumpetDrawDirection = 1;
    BYTE trumpetDrawPlane = -1;
    UBYTE trumpetDrawHeight = 4;
    UBYTE trumpetMoveSequence = 0;
    UBYTE trumpetBackFade = 0;

    BYTE direction = -1;

    UWORD modPosition = 0;
    UBYTE isStep = 1;
    UBYTE paletteIndex = 0;

    if (isMouseDown()) return;

    // get Asset ready;
    fogAsset = getFogAsset();
    asset_moveToChip(backgroundAsset);
    asset_moveToChip(fogAsset);
    asset_moveToChip(trumpetAsset);
    

    //setup copper for the player background fade
    copper_start();
    y = 0;  
    for (i = 120; i < 254; i++) {
       copper_setLineSection(i);
    }

    copper_end();

    palette_setBlack(32);
    screen_enableDoubleBuffer();
    WaitTOF();


    // attach sprites and set the sprite position
    // Note: the top left corner of the screen is the DIWSTRT_VALUE ?
    for (i = 0; i < 8; i++) {
        sprite_attach(getSpriteData(i), i);
        // move sprites off screen
        sprite_setPosition(getSpriteData(i), 460, trumpet_y - trumpet_offsetY[i], trumpet_heights[i]);
    }

    sprites_on();
    //sprites_off();

    setSpotLightPalette(palette1);
    palette_startColorCycle();
    
    if (fadeIn){
        for (i = 0; i < 32; i++) {
            copper_setColorValue(i, 0x000);
        }
    }
    
    screen_drawAsset(backgroundAsset, 0, 0, 320, 256, 0, 0);
   
    if (fogActive){
        screen_overrideBitPlane(3, (APTR) getAssetData(fogAsset));
    }


    // inital screen setup done -> copy to the buffer so they both contain the same data.
    // in this scene, we don't touch the first 3 bitplanes anymore, so we don't have to 
    // copy them to the buffer on each flip
    screen_copyToBuffer();


    // frame loop
	while(loop){
        
        WaitTOF();

        if (fadeIn && currFrame < 128){
            setBackgroundBlend(254-(currFrame<<1));
        }

         if (fadeOut && fadeOut<255){
            fadeOut++;
            setBackgroundBlend(fadeOut);
        }

        if (trumpetMoveSequence){
            // sprites are visible
            trumpet_shiftXPosition += 2;

            switch (trumpetMoveSequence){
                case 1:
                     // start position
                    for (i = 0; i < 8; i++) {
                        sprite_setPosition(getSpriteData(i), getSpriteXOffset_contracted(i), trumpet_y - trumpet_offsetY[i], trumpet_heights[i]);
                    }  
                    break;
                case 2:
                    // appear
                    for (i = 0; i < 8; i++) {
                        sprite_setPosition(getSpriteData(i), getSpriteXOffset_appear(i),  getSpriteYOffset_appear(i), trumpet_heights[i]);
                    }  
                    if (trumpet_shiftXPosition >= 100) {
                            trumpet_shiftXPosition = 100;
                    }
                    break; 
                case 3:  
                    // slow Up  
                    for (i = 0; i < 8; i++) {
                        sprite_setPosition(getSpriteData(i), getSpriteXOffset_expanded(i), getSpriteYOffset_up(i), trumpet_heights[i]);
                    }  
                    if (trumpet_shiftXPosition >= 256) {
                        trumpet_shiftXPosition = 256;
                    }    
                    break; 
                case 4:   
                    // up/down
                    if (trumpetMoveSequence == 4){
                        for (i = 0; i < 8; i++) {
                            sprite_setPosition(getSpriteData(i), getSpriteXOffset_expanded(i), getSpriteYOffset_swing(i), trumpet_heights[i]);
                        }  
                        if (trumpet_shiftXPosition >= 100) {
                            trumpet_shiftXPosition = 0;
                            trumpet_shiftYDirection = -trumpet_shiftYDirection;
                        }
                    }   
                    break; 
                case 5:  
                    // left/right
                    if (trumpetMoveSequence == 5){
                        for (i = 0; i < 8; i++) {
                            sprite_setPosition(getSpriteData(i), getSpriteXOffset_swing(i),  getSpriteYOffset_expanded(i), trumpet_heights[i]);
                        }  
                        if (trumpet_shiftXPosition >= 100) {
                            trumpet_shiftXPosition = 0;
                        }
                    }  
                    break;   
                case 6:
                    // circle
                    if (trumpetMoveSequence == 6){
                        for (i = 0; i < 8; i++) {
                            sprite_setPosition(getSpriteData(i), getSpriteXOffset_swing(i), getSpriteYOffset_swing(i), trumpet_heights[i]);
                        }  
                        if (trumpet_shiftXPosition >= 100) {
                            trumpet_shiftXPosition = 0;
                            trumpet_shiftYDirection = -trumpet_shiftYDirection;
                        }
                    } 
                    break;       
            }


        }

        if (spotlightActive){
            blendAmount += blendSpeed;
            if (blendAmount > 250) {
                blendSpeed = -1;
            }
            if (blendAmount <=0) {
                blendAmount = 0;
                blendSpeed = 1;
                blendColorIndex++;
                if (blendColorIndex > 2) {
                    blendColorIndex = 0;
                }
                blendColorLight = blendColors[blendColorIndex << 1];
                blendColorDark = blendColors[(blendColorIndex << 1) + 1];
            }
            setSpotLightBlend(blendAmount);

            xd = math_sin(currAngle%180);
            xd2 = math_sin(currAngle >> 1);
        }
        
        if (trumpetBackFade>0){
            trumpetBackFade++;
            if (trumpetBackFade == 2) copper_setBackBlock(0x144,0x133,0x033,0x022,0x011,0x000);
            if (trumpetBackFade == 6) copper_setBackBlock(0x133,0x033,0x022,0x011,0x000,0x000);
            if (trumpetBackFade == 10) copper_setBackBlock(0x033,0x022,0x011,0x000,0x000,0x000);
            if (trumpetBackFade == 14) copper_setBackBlock(0x022,0x011,0x000,0x000,0x000,0x000);
            if (trumpetBackFade == 16) copper_setBackBlock(0x011,0x000,0x000,0x000,0x000,0x000);
            if (trumpetBackFade == 20) copper_setBackBlock(0x000,0x000,0x000,0x000,0x000,0x000);
            if (trumpetBackFade > 24) trumpetBackFade = 0;
        }

        palette_updateColorCycle();
        

        currAngle++;
        currFrame++;
        if (currAngle >= 360) {
            currAngle = 0;
        }
        
            
        OwnBlitter();
                
            if (spotlightActive){
                // draw spotlights
                if (spotlightFill) bitplane_clear(spotLightPlane); 
                
                screen_drawLine_on_plane_with_checks(150, 0, 100 + xd, 250, spotLightPlane);
                screen_drawLine_on_plane_with_checks(150, 0, 160 + xd, 250, spotLightPlane);

                screen_drawLine_on_plane_with_checks(152, 0, 120 - xd2, 250, spotLightPlane);
                screen_drawLine_on_plane_with_checks(152, 0, 170 - xd2, 250, spotLightPlane);
                
                if (spotlightFill && useFill){
                    // TODO: disable on slow machines?
                    bitplane_fillArea_simple(spotLightPlane,0, 1, 304, 255, 0, 0);
                }
            }

            switch (currFrame){
                case PLAYER_APPEAR:
                    // start drawing trumpet appear
                    trumpetDrawPlane = 0;
                    copper_setBackBlock(0x155,0x144,0x133,0x033,0x022,0x011);
                    break;
                case PLAYER_FADEBACKGROUND:
                    trumpetBackFade = 1;
                    break;                    
                case PLAYER_TOSPRITE:
                    bitplane_clear(spotLightPlane);
                    screen_overrideBitPlane(3, 0);
                    spotLightPlane = 3;
                    fogActive = FALSE;
                    setSpotLightPalette(palette1);
                    activateSprites();
                    break;
                case PLAYER_TOSPRITE2:
                    bitplane_clear(4);
                    break;
                case PLAYER_TOSPRITE3:
                    screen_drawAsset(backgroundAsset, 0, 0, 320, 256, 0, 0);
                    trumpetMoveSequence = 1;
                    break;
                case PLAYER_TOSPRITE4:  
                    screen_drawAsset(backgroundAsset, 0, 0, 320, 256, 0, 0);
                    break;
                case PLAYER_TURN:
                    trumpet_shiftXPosition = 0;
                    trumpetMoveSequence = 2;
                    break;
                case PLAYER_UP:
                    trumpet_shiftXPosition = 0;
                    trumpetMoveSequence = 3;
                    break;   
                case PLAYER_SWING:
                    trumpet_shiftXPosition = 0;
                    trumpetMoveSequence = 5;
                    break;   
                case PLAYER_SWING2:
                    trumpet_shiftXPosition = 50;
                    trumpet_shiftYDirection = 1;
                    trumpetMoveSequence = 6;
                    break;    
                case STROBO:
                    spotlightFill = FALSE;
                    break;   
            }


            if (trumpetDrawPlane >= 0){
                // draw same blit twice due to simplified double buffering
                trumpetDrawSegment = trumpetDrawCount>>1;
                y = trumpetDrawSegment * trumpetDrawHeight;
                if (trumpetDrawDirection == 1){
                    y = y*2;
                }else{
                    y = 360 - y*2 - trumpetDrawHeight;
                }
                if (y>=180){
                    trumpetDrawDirection = -1;
                    y = 0;
                }
                if (y < 0){
                    trumpetDrawDirection = 1;
                    trumpetDrawPlane++;
                    trumpetDrawCount = 0;
                    trumpetDrawSegment = 0;
                    y = 0;
                }

                if (trumpetDrawPlane > 2){
                    // done drawing player
                    trumpetDrawPlane = -1;
                }else{
                    screen_drawAssetPlane(trumpetAsset, trumpetDrawPlane , 112, 76 + y, 80, trumpetDrawHeight, trumpetDrawPlane, 0, y);
                    trumpetDrawCount++;
                }
            }


            

            if (fogActive){
                if (spotLightPlane == 4){
                    fogY++;
                    if (fogY >512) {
                        fogY = 0;
                    }
                    screen_overrideBitPlane(3, getAssetData(fogAsset) + fogY * 40);
                }
            }
           
        DisownBlitter();

        screen_flip();

        if (isStep && modPosition > 47){
            stepFrame=0;

            if (modPosition==62){
                palette_setBlack(32);
                trumpetMoveSequence = 0;
               }
        }

        if (modPosition > 47 && modPosition < 62){
            if (stepFrame == 0 || modPosition>55 && stepFrame%6==0 || modPosition>59 && stepFrame%3==0){
               paletteIndex++;
               if (paletteIndex%2 == 1){
                palette_setBlack(8);
               }else{
                palette_set(palette1,8);
               }
            }
            stepFrame++;
        }
        
        if (modPosition == 62){
            palette_setBlack(32);
            copper_reset();
        }

        isStep = mod_isStep();
        if (isStep){
            modPosition++;
        }
            

        
		if(modPosition>63) loop = FALSE;

        if (loop == FALSE){
            for (i = 0; i < 8; i++) {
                sprite_detach(i);
            }
            screen_overrideBitPlane(3, 0);
            asset_free(backgroundAsset);
            asset_free(fogAsset);
            asset_free(trumpetAsset);
        }	
        
	}

}