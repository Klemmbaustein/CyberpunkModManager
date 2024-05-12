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

# ./
cd ../../..