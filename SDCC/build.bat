@ECHO OFF

REM compile...
sdcc -c main.c 
sdcc -c wheelwriter.c
sdcc -c uart1.c
sdcc -c uart2.c
sdcc -c ww-uart3.c
sdcc -c ww-uart4.c

REM link...
sdcc main.c wheelwriter.rel uart1.rel uart2.rel ww-uart3.rel ww-uart4.rel

REM generate HEX file...
packihx main.ihx > teletype.hex

REM optional cleanup...
del *.asm
del *.ihx
del *.lk
del *.lst
del *.map
del *.mem
del *.rel
del *.rst
del *.sym

pause
