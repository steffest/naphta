#include <exec/types.h>
#include <hardware/custom.h>
#include <graphics/gfxbase.h>
#include "app.h"
#include "ptplayer/ptplayer.h"

int file_length = 0;
static struct Custom custom;
extern far UBYTE mt_Enable;
extern far UBYTE mt_E8Trigger;
UBYTE *mod_data;
UBYTE E8_value = 0;

BOOL mod_playAsset(int index, UBYTE start_pos){
    struct GfxBase *GfxBase;

    void *p_samples = NULL;
    BOOL is_pal;

    file_length = 0;
    mod_data = getAssetData(index);
    //is_pal = (((struct GfxBase *) GfxBase)->DisplayFlags & PAL) == PAL;
    //printf("PAL: %d\n", is_pal);
    is_pal = 1;

    mt_install();
    mt_init(&custom, mod_data, p_samples, start_pos);
    mt_Enable = 1;

    if (WRITE_DEBUG) printf("Playing mod\n");
    return TRUE;
}

void mod_stop(){
    if (WRITE_DEBUG) printf("Cleaning up\n");
    //if (file_length>0) FreeMem(mod_data, file_length);
    //mt_mastervol(&custom, 0);
    Delay(10);
    mt_Enable = 0;
    mt_end(&custom);
    mt_remove();
}

UBYTE mod_E8Value(){
    return mt_E8Trigger;
}

UBYTE mod_isStep(){
    if (E8_value != mt_E8Trigger){
        E8_value = mt_E8Trigger;
        return 1;
    }
    return 0;
}
