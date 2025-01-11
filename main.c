#include <exec/memory.h>
#include "app.h"

#define TASK_PRIORITY 40

BOOL playMusic = TRUE;
BYTE modIndex;

int main(int argc, char **argv){
    
    if (WRITE_DEBUG) system_printInfo();

    if (!display_open()) {
        puts("Could not open display");
        return 1;
    }

    SetTaskPri(FindTask(NULL), TASK_PRIORITY);
    
    copper_init();
    screen_init(SCREENWIDTH, SCREENHEIGHT, 5, TRUE);
    copper_activate();

    loader_show();

    if (playMusic){
        loader_next();
        modIndex=-1;
        if (system_isSomewhatHighEnd()) modIndex = asset_loadFile("naphta-high.mod", MEMF_CHIP);
        if (modIndex < 0) {
            // running from disk or on low specs
            modIndex = asset_loadFile("naphta-low.mod", MEMF_CHIP);
        }
        loader_next();
    }

    intro_preload();
    sax_preload();
    grid2_preload();
    grid_preload();
    trumpetplayer_preload();
    dancer_preload();

    if (WRITE_DEBUG){
        printf("Asset preload done\n");
        printf("Free Chip memory: %d\n", AvailMem(MEMF_CHIP));
        printf("Free Fast memory: %d\n", AvailMem(MEMF_FAST));
    }

    if (playMusic) mod_playAsset(modIndex,0);
    loader_hide();

    intro();
    sax();
    grid2();
    grid(1);
    trumpetplayer();
    grid(2);
    dancer();
    grid(3);
    end();
    
    if (playMusic){
        mod_stop();
    }

    free_assets();
    screen_close();
    display_close();
    return 0;
}

