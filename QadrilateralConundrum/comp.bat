set PATH=c:\PSYQ\BIN
set PSYQ_PATH=c:\PSYQ\BIN

set prog=MAIN
set address=$80100000
set param=-DCDROM_RELEASE
set lib=..\..\MyPsxLib

cls
ccpsx -I%lib%\. %param% -O2 -Xo%address% %prog%.c %lib%\DataManager.c %lib%\System.c %lib%\Sprite.c -o%prog%.cpe,,%prog%.map
pause

cpe2x %prog%.cpe
del %prog%.psx
del %prog%.cpe
ren %prog%.exe %prog%.PSX
pause
