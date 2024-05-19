# Dependencies/KlemmUI
cd Dependencies/KlemmUI
./Setup.ps1

cp lib/Release/SDl2.dll ../../App

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

# 7zip is insane.

if ($args[0] -eq "CI_BUILD")
{
	cd ../../..
	Invoke-WebRequest https://www.7-zip.org/a/7z2405-extra.7z -OutFile 7zip.7z
	7z x -y 7zip.7z -o7zipFiles/
	mkdir -f App/app/bin
	cp 7ZipFiles/7za.exe App/app/bin/
	rm -r -fo 7zipFiles
	rm 7zip.7z
}
else
{
	# Dependencies/7zip/CPP/7zip
	cd ../../7zip/CPP/7zip
	nmake
	mkdir ../../../../App/app/bin -f
	cp Bundles/Alone/o/7za.exe ../../../../App/app/bin/
	cd ../../../..
}

# ./
