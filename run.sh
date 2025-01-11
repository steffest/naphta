killall fs-uae 
mkdir -p ./disk
rm -rf ./disk/*
mkdir -p ./disk/data
cp -r ./data/* ./disk/data/
#./tools/Shrinkler_osx -w -d -1 naphta5.mod naphta5.compressed
#cp naphta4time.mod ./disk/
cp naphta-low.mod ./disk/
rm ./naphta.exe
cp main ./naphta.exe
./tools/exe2adf -i naphta.exe -a naphta.adf -d ./disk/ -l NAPTHA -k -t wait.txt
open ./run.fs-uae
#open ./naphta.adf