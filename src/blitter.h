void blit_object(struct Ratr0TileSheet *bobs,struct Ratr0TileSheet *background,int tilex, int tiley,int dstx, int dsty);
void blit_objectscreen(struct Ratr0TileSheet *bobs,int tilex, int tiley,int dstx, int dsty);
void blit_asset(UBYTE index, int dstx, int dsty);
void blitter_fillRect(UBYTE *target, int x, int y, int width, int height, UBYTE fill);