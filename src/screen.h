#include <exec/types.h>

struct DScreen {
    USHORT width;
    USHORT height;
    UBYTE depth;
    USHORT size;
    USHORT planeSize;
    UBYTE doubleBuffer;
    UBYTE activeScreen;
    UBYTE *canvas;
    UBYTE *canvas2;
};

struct Point {
    USHORT x, y;
};

void screen_copyToBuffer(void);
void screen_flip(void);
void screen_disableDoubleBuffer(void);
void screen_enableDoubleBuffer(void);
char screen_init(USHORT width, USHORT height, UBYTE bitplaneCount, BOOL doubleBuffer);
void screen_close(void);
void screen_clear(void);
void screen_drawPixel(USHORT x, USHORT y, UBYTE color);
void screen_drawRect(USHORT x, USHORT y, USHORT width, USHORT height, UBYTE color);
void screen_drawImage(USHORT index, USHORT x, USHORT y);
void screen_drawAsset(UBYTE index, WORD x, WORD y, USHORT width, USHORT height, UWORD srcX, UWORD srcY);
void screen_drawAsset2(BYTE index, USHORT x, USHORT y);
void screen_drawLine_on_plane(USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE plane);
void screen_drawLine_on_plane_with_checks(USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE plane);
void screen_drawTriangle(USHORT x1, USHORT y1,USHORT x2, USHORT y2, USHORT x3, USHORT y3, UBYTE filled, UBYTE plane);
void screen_drawAssetPlane_simple(UBYTE index, UBYTE plane);
void screen_drawAssetPlane_y(UBYTE index, UBYTE plane, WORD y, USHORT height);
void screen_drawAssetPlane(UBYTE index, UBYTE dstPlane, WORD x, WORD y, USHORT width, USHORT height, UBYTE srcPlane, UWORD srcX, UWORD srcY);
void screen_overrideBitPlane(UBYTE plane, APTR address);
struct DScreen *screen_getScreen(void);
UBYTE *screen_getCanvas(void);