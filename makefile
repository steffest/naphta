OBJS=main.o \
src/math.o \
src/system.o \
src/input.o \
src/loader.o \
src/screen.o \
src/bitplane.o \
src/display.o \
src/blitter.o \
src/copper.o \
src/sprite.o \
src/palette.o \
src/modPlay.o \
src/assetManager.o \
parts/trumpetplayer.o \
src/ptPlayer/ptPlayer.o \
parts/intro.o \
parts/sax.o \
parts/grid.o \
parts/grid2.o \
parts/dancer.o \
parts/end.o

naphta: $(OBJS)
   sc link to main $(OBJS)

main.o: main.c
display.o: src/display.c
modPlay.o: src/modPlay.c
assetManager.o: src/assetManager.c
copper.o: src/copper.c
sprite.o: src/sprite.c
blitter.o: src/blitter.c
loader.o: src/loader.c
screen.o: src/screen.c
bitplane.o: src/bitplane.c
system.o: src/system.c
input.o: src/input.c
palette.o: src/palette.c
math.o: src/math.c

intro.o: parts/intro.c
sax.o: parts/sax.c
trumpetplayer.o: parts/trumpetplayer.c
grid.o: parts/grid.c
grid2.o: parts/grid2.c
dancer.o: parts/dancer.c
end.o: parts/end.c
   

