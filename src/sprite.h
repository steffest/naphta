void sprite_init(void);
void sprite_attach(unsigned short *sprite, char spriteIndex);
void sprite_detach(char spriteIndex);
void sprite_setPosition(unsigned short *sprite_data, unsigned short hstart, unsigned short vstart, unsigned short height);
void sprites_on(void);
void sprites_off(void);