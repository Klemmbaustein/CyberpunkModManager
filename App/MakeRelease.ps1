# Copies all neccessary files into the App/Release directory.

rm  -r -fo Release/

mkdir Release/
cp app Release/ -r -fo
cp App.exe Release/CyberpunkModManager.exe

cp SDL2.dll Release/SDL2.dll
