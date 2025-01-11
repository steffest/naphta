#include "app.h"

static UWORD palette1[] = {0x000,0x111,0x122,0x244,0x365,0x587,0x7a9,0xacb, // background
                            0X000,0X122,0X133,0X255,0X387,0X5a9,0X7cb,0Xbed, // fog
                            0X223,0X233,0X234,0X357,0X589,0X7ab,0X9dd,0Xcff, // spotlight
                            0X223,0X335,0X356,0X479,0X6ac,0X8ce,0Xaff,0Xeff  // fog under sportlight
                            };

static struct Ratr0TileSheet bobs;

static void cleanup(void){
    ratr0_free_tilesheet_data(&bobs);
}

void test_main(void){

    UBYTE num_colors;
    int assetIndex = 0;

    
    if (!ratr0_read_tilesheet("rodland_bobs.ts", &bobs)) {
        puts("Could not read bob sheet");
        cleanup();
        return 1;
    }

    //loader_next();
    assetIndex = asset_loadImage("data/spotlight1.planes", 320, 256, 3, MEMF_ANY);
    //loader_next();

    Delay(20);
    


    num_colors = 1 << SCREENDEPTH;
    palette_set(palette1,num_colors);
    WaitTOF();

    sprites_off();

    asset_moveToChip(assetIndex);

    OwnBlitter();
    screen_drawAsset2(assetIndex, 0, 0);
    blit_objectscreen(&bobs, 1, 0, 140, 100);
    DisownBlitter();
    screen_flip();


    waitMouse();

    cleanup();


}