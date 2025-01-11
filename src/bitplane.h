#include <exec/types.h>

void bitplane_clear(UBYTE plane);
void bitplane_clearRect(UBYTE plane, UWORD x, UWORD y, UWORD w, UWORD h);
void bitplane_drawRect(UBYTE plane, UWORD x, UWORD y, UWORD w, UWORD h, BOOL fill);
void bitplane_fillRect(UBYTE plane, USHORT dst_x, USHORT dst_y, USHORT width, USHORT height, UBYTE set_bits);
void bitplane_fillArea_simple(UBYTE plane, USHORT x1, USHORT y1, USHORT x2, USHORT y2, UBYTE exclusive, UBYTE fill_carry_input);
void bitplane_drawLine(UBYTE plane, UWORD x1, UWORD y1, UWORD x2, UWORD y2, UBYTE singlePixelPerLine);