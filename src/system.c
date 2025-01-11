
#include <exec/memory.h>
#include <exec/execbase.h>

extern struct ExecBase *SysBase;

int getcpu(void){
  UWORD attnflags = SysBase->AttnFlags;

  if (attnflags & 0x80) return 68060;
  if (attnflags & AFF_68040) return 68040;
  if (attnflags & AFF_68030) return 68030;
  if (attnflags & AFF_68020) return 68020;
  if (attnflags & AFF_68010) return 68010;
  return 68000;
}

void system_printInfo(void) {
    //int macChip = MaxLocMem();
    int chipMem = AvailMem(MEMF_CHIP);
    int fastMem = AvailMem(MEMF_FAST);
    printf("Memory available: Chip %d Fast  %d\n", chipMem,fastMem);
    //printf("Max Chip %d\n", SysBase->MaxLocMem);
    printf("VBlank: %d\n", SysBase->VBlankFrequency);
    printf("Kickstart v%d\n", SysBase->LibNode.lib_Version);
    printf("CPU: %d\n", getcpu());
}

BOOL system_isSomewhatHighEnd(void) {
    int chipMem = AvailMem(MEMF_CHIP);
    return (chipMem > 1000000 && getcpu()>68010) ? TRUE : FALSE;
}

char system_isChipMem(void *mem) {
    return SysBase->MaxLocMem >= (ULONG)mem ? 1 : 0;
}	