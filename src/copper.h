void copper_init();
void copper_reset(void);
void copper_activate();
void copper_restore();
void copper_setColorValue(unsigned short index, unsigned short value);
void copper_setBitPlane(unsigned short index, unsigned short value);
void copper_setSpritePointer(unsigned short *sprite, char spriteIndex);
void copper_setValue(unsigned short index, unsigned short value);
void copper_setExtention(unsigned short index, unsigned short value);
void copper_start(void);
void copper_setExtentionIndex(UWORD index);
UWORD copper_waitForLine(UWORD line);
UWORD copper_setColor(UWORD index, UWORD color);
void copper_setLineSection(UWORD line);
UWORD copper_end(void);
void copper_setBackBlock(UWORD color,UWORD color2,UWORD color3,UWORD color4,UWORD color5,UWORD color6);
void copper_setGrid();
void copper_setGridValue(UWORD value, UWORD index);
void copper_setGridColor(UBYTE g);
void copper_setGridColors(UWORD *screen);
void copper_setGridScreen(UWORD *screen);
void copper_setGrid2Colors(UBYTE *screen);
void copper_setIntro(void);