
cd disk/data
del *.planes
del *.chunky
del *.compressed
cd ../../
xcopy /y data disk\data
%~dp0/tools/Shrinkler.exe -9 -t "NAPHTA is decrunching..." main main.exe
%~dp0/tools/exe2adf.exe -i main.exe -a main.adf -d ./disk/
start run.uae