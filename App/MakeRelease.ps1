# Copies all neccessary files into the App/Release directory.

if (Test-Path "Release") {
   rm "Release" -r -fo
}

mkdir Release/
cp app Release/ -r -fo
cp ../build/Release/ModManager.exe Release/CyberpunkModManager.exe
cp ../LICENSE.txt Release/
cp ../build/Release/SDL2.dll Release/SDL2.dll
