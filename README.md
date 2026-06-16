# PureDOOM for SF32
PureDOOM port for SiFli SF32 chips.

Run on sf32lb52-lchspi-ulp, other board may work but not tested.
 
## How to build
1. Run export.ps1 in your sifli-sdk dir to init env.
2. Cd project and run scons --board=sf32lb52-lchspi-ulp

## Supported Features:
1. FAT32 SD card for wad file
2. Video display
3. Audio output

## Unsupported Features for now:
1. Key input
2. Network

## Known issues:
1. FPS low
