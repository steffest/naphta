#include "app.h"
#include <hardware/cia.h>

#define CIAAPRA 0xBFE001
struct CIA *cia = (struct CIA *) CIAAPRA;

void waitMouse(void){
    BOOL loop = TRUE;
    while(loop){
        WaitTOF();
        if ((cia->ciapra & 0x0040) == 0) loop = FALSE; // port 1
        if ((cia->ciapra & 0x0080) == 0) loop = FALSE; // port 2
    }

}

BOOL isMouseDown(void){
    return ((cia->ciapra & 0x0040) == 0 ) || ((cia->ciapra & 0x0080) == 0);
}