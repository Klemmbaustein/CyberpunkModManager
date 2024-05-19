# Copies all neccessary files into the App/Release directory.

if (Test-Path "Release") {
   rm "Release" -r -fo
}

mkdir Release/
cp app Release/ -r -fo
cp App.exe Release/CyberpunkModManager.exe
cp ../LICENSE.txt Release/
cp SDL2.dll Release/SDL2.dll
