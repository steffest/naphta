#include <hardware/custom.h>
#include "app.h"

extern struct Custom far custom;

/*
color mapping in a 5-bitplane screen

color 17-19: sprite 0 and 1
color 21-23: sprite 2 and 3
color 25-27: sprite 4 and 5
color 29-31: sprite 6 and 7

*/

static unsigned short __chip NULL_SPRITE_DATA[] = {
    0x0000, 0x0000,
    0x0000, 0x0000
};

void sprite_init(void){
    int i;
    // point sprites 0-7 to nothing
    for (i = 0; i < 8; i++) {
        copper_setSpritePointer(NULL_SPRITE_DATA, i);
    }
}

void sprite_attach(unsigned short *sprite, char spriteIndex){
    copper_setSpritePointer(sprite, spriteIndex);
}

void sprite_detach(char spriteIndex){
    copper_setSpritePointer(NULL_SPRITE_DATA, spriteIndex);
}

// sets the first 4 words of the sprite data to the position/size of the sprite
void sprite_setPosition(unsigned short *sprite_data, unsigned short hstart, unsigned short vstart, unsigned short height){
    unsigned short vstop = vstart + height;
    sprite_data[0] = ((vstart & 0xff) << 8) | ((hstart >> 1) & 0xff);
    // vstop + high bit of vstart + low bit of hstart
    sprite_data[1] = ((vstop & 0xff) << 8) |  // vstop 8 low bits
        ((vstart >> 8) & 1) << 2 |  // vstart high bit
        ((vstop >> 8) & 1) << 1 |   // vstop high bit
        (hstart & 1) |              // hstart low bit
        sprite_data[1] & 0x80;      // preserve attach bit
}

void sprites_on(void){
    custom.dmacon = 0x0400; // blitter nasty - does that have any measurable effect?
}

void sprites_off(void){
    custom.dmacon = 0x20; // disable sprite dma - bit 5
}