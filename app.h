#include <exec/memory.h>

#define SCREENWIDTH 320
#define SCREENHEIGHT 256
#define SCREENDEPTH 5
#define PLANESIZE (SCREENWIDTH * SCREENHEIGHT / 8)
#define WRITE_DEBUG 0

#define kBitsPerByte 0x8
#define kBitsPerWord 0x10
#define kBytesPerWord 0x2
#define kUWordMax 0xFFFF

#include "src/amiga_custom.h"
#include "src/system.h"
#include "src/input.h"
#include "src/screen.h"
#include "src/bitplane.h"
#include "src/assetManager.h"
#include "src/display.h"
#include "src/modPlay.h"
#include "src/copper.h"
#include "src/sprite.h"
#include "src/blitter.h"
#include "src/palette.h"
#include "src/math.h"

#include "parts/parts.h"