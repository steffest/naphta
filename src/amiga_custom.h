#define FMODE         0x1fc

#define BLTDDAT       0x000
#define DMACONR       0x002
#define VPOSR         0x004
#define VHPOSR        0x006

#define DIWSTRT       0x08e
#define DIWSTOP       0x090
#define DDFSTRT       0x092
#define DDFSTOP       0x094
#define DMACON        0x096
#define BPL1PTH       0x0e0
#define BPL1PTL       0x0e2
#define BPL2PTH       0x0e4
#define BPL2PTL       0x0e6
#define BPL3PTH       0x0e8
#define BPL3PTL       0x0ea
#define BPL4PTH       0x0ec
#define BPL4PTL       0x0ee
#define BPL5PTH       0x0f0
#define BPL5PTL       0x0f2
#define BPL6PTH       0x0f4
#define BPL6PTL       0x0f6


#define BPLCON0       0x100
#define BPLCON1       0x102
#define BPLCON2       0x104
#define BPLCON3       0x106
#define BPL1MOD       0x108
#define BPL2MOD       0x10a
#define SPR0PTH       0x120
#define SPR0PTL       0x122
#define SPR1PTH       0x124
#define SPR1PTL       0x126
#define SPR2PTH       0x128
#define SPR2PTL       0x12a
#define SPR3PTH       0x12c
#define SPR3PTL       0x12e
#define SPR4PTH       0x130
#define SPR4PTL       0x132
#define SPR5PTH       0x134
#define SPR5PTL       0x136
#define SPR6PTH       0x138
#define SPR6PTL       0x13a
#define SPR7PTH       0x13c
#define SPR7PTL       0x13e

#define COLOR00       0x180
#define COLOR01       0x182
#define COLOR02       0x184
#define COLOR03       0x186
#define COLOR04       0x188
#define COLOR05       0x18a
#define COLOR06       0x18c
#define COLOR07       0x18e
#define COLOR08       0x190
#define COLOR09       0x192
#define COLOR10       0x194
#define COLOR11       0x196
#define COLOR12       0x198
#define COLOR13       0x19a
#define COLOR14       0x19c
#define COLOR15       0x19e
#define COLOR16       0x1a0
#define COLOR17       0x1a2
#define COLOR18       0x1a4
#define COLOR19       0x1a6
#define COLOR20       0x1a8
#define COLOR21       0x1aa
#define COLOR22       0x1ac
#define COLOR23       0x1ae
#define COLOR24       0x1b0
#define COLOR25       0x1b2
#define COLOR26       0x1b4
#define COLOR27       0x1b6
#define COLOR28       0x1b8
#define COLOR29       0x1ba
#define COLOR30       0x1bc
#define COLOR31       0x1be

#define PRA_FIR0_BIT            (1 << 6)
#define BPLCON0_COMPOSITE_COLOR (1 << 9)

#define BLTCON0_ASH0_SHF 0xC
#define BLTCON0_USEA     0x0800
#define BLTCON0_USEB     0x0400
#define BLTCON0_USEC     0x0200
#define BLTCON0_USED     0x0100
#define BLTCON0_LF0_SHF  0x0
#define BLTCON1_BSH0_SHF 0xC
#define BLTCON1_TEX0_SHF 0xC
#define BLTCON1_SIGN_SHF 0x6
#define BLTCON1_AUL_SHF  0x2
#define BLTCON1_SING_SHF 0x1
#define BLTCON1_IFE      0x0008
#define BLTCON1_DESC     0x0002
#define BLTCON1_LINE     0x0001
#define BLTSIZE_H0_SHF   0x6
#define BLTSIZE_W0_SHF   0x0
#define BPLCON0_BPU_SHF  0xC
#define BPLCON0_COLOR    0x0200
#define BPLCON3_SPRES_Shf 0x6
#define COPCON_CDANG     0x2
#define DIWHIGH_H10_SHF  0xD
#define DIWHIGH_V8_SHF   0x8
#define DIWSTOP_V0_SHF   0x8
#define DIWSTRT_V0_SHF   0x8
#define DMACON_SET       0x8000
#define DMACON_CLEARALL  0x7FFF
#define DMACON_BLITPRI   0x0400
#define DMACON_DMAEN     0x0200
#define DMACON_BPLEN     0x0100
#define DMACON_COPEN     0x0080
#define DMACON_BLTEN     0x0040
#define DMACON_SPREN     0x0020
#define DMACONR_BBUSY    0x4000
#define INTENA_SET       0x8000
#define INTENA_CLEARALL  0x7FFF
#define INTENA_PORTS     0x0008
#define INTREQ_SET       0x8000
#define INTREQ_CLEARALL  0x7FFF