# Dependencies/KlemmUI
cd Dependencies/KlemmUI
./Setup.ps1

# Dependencies/curl
cd ../curl

cmake -DCURL_USE_SCHANNEL=on -DBUILD_SHARED_LIBS=off -DCMAKE_C_COMPILER=cl -B Build

# Dependencies/curl/Build
cd Build
msbuild.exe lib/libcurl_static.vcxproj /p:Configuration=Release /p:Platform=x64
msbuild.exe lib/libcurl_static.vcxproj /p:Configuration=Debug /p:Platform=x64

# Dependencies/libwebp
cd ../../libwebp
cmake -S . -B Build/
# Dependencies/libwebp/Build
cd Build/
msbuild.exe webp.vcxproj /p:Configuration=Release /p:Platform=x64

# Dependencies/7zip/CPP/7zip
cd ../../7zip/CPP/7zip

# 7zip is insane.
nmake
mkdir ../../../../App/app/bin -f
cp Bundles/Alone/o/7za.exe ../../../../App/app/bin/

# ./
cd ../../../..