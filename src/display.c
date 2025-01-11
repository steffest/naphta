#include <graphics/gfxbase.h>
#include <intuition/screens.h>
#include "app.h"

extern struct GfxBase *GfxBase;
static struct NewScreen screenProperties = {
        0, 0, SCREENWIDTH, SCREENHEIGHT, 1, 0, 0, 0,
	    CUSTOMSCREEN | CUSTOMBITMAP,
	    NULL, NULL, NULL, NULL,
     };

int screen;    

char display_open(void){
     //screen = OpenScreen((void *)(&screenProperties));
    // note: We only open a screen to prevent keyboard input being captrured by the active window ...

    LoadView(NULL);  // clear display, reset hardware registers
    WaitTOF();       // 2 WaitTOFs to wait for 1. long frame and
    WaitTOF();       // 2. short frame copper lists to finish (if interlaced)
}

void display_close(void){
    // restore original display
    LoadView(((struct GfxBase *) GfxBase)->ActiView);
    WaitTOF();
    WaitTOF();
    copper_restore();
    RethinkDisplay();

    //if (screen) CloseScreen(screen);
}

char display_isPAL(void){
    //return 1;
    return (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
}
