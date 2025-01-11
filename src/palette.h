#include <exec/types.h>
void palette_init(void);
void palette_set(UWORD *palette, UBYTE size);
void palette_setBlack(UBYTE size);
void palette_setColor(UBYTE index, UWORD color);
void palette_setColorCycle(UBYTE index, UWORD *colors, UBYTE size, UBYTE speed);
void palette_updateColorCycle(void);
void palette_stopColorCycle(void);
void palette_startColorCycle(void);
void palette_setColorCycleIndex(UBYTE colorIndex,UBYTE cycleIndex);
void palette_fluctuateColor(UBYTE index, UWORD startColor, BYTE min, BYTE max, UBYTE speed, BYTE startDirection);
void palette_removeColorCycle(UBYTE index);
void palette_addColorCycle(UBYTE index, UWORD startColor);
void palette_setBlendColor(UBYTE index, UWORD color, UBYTE blend);
UWORD palette_blendColor(UWORD color1, UWORD color2, UBYTE blend);
UWORD palette_blendRGB(UBYTE red1, UBYTE green1, UBYTE blue1, UBYTE red2, UBYTE green2, UBYTE blue2, UBYTE blend);
UWORD palette_randomBrightColor(void);
UWORD palette_getBrightColor(UBYTE index);