#include "app.h"

#define WIDTH 100
#define HEIGHT 6
UBYTE x = (SCREENWIDTH - WIDTH)>>1;
UBYTE y = (SCREENHEIGHT - HEIGHT)>>1;

UBYTE loaderProgress = 0;

void loader_show(void){
    screen_disableDoubleBuffer();
    palette_setColor(0, 0x000);
    palette_setColor(1, 0xfff);
    bitplane_drawRect(0, x, y, WIDTH+4, HEIGHT+4, FALSE);
}

void loader_hide(void){
    bitplane_clear(0);
    screen_enableDoubleBuffer();
}

void loader_setProgress(UBYTE progress){
    if (progress > 100) progress = 100;
    loaderProgress = progress;
    bitplane_drawRect(0, x+2, y+2, progress, HEIGHT, TRUE);
}   

void loader_next(void){
    loaderProgress += 2;
    loader_setProgress(loaderProgress);
}

