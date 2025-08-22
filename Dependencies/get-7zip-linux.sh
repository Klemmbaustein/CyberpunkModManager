#! /usr/bin/bash
# I don't like this, but I can't find a way to build 7zip for linux.

wget https://www.7-zip.org/a/7z2405-linux-x64.tar.xz

mkdir -p 7zip-download/
tar -xf 7z2405-linux-x64.tar.xz -C 7zip-download/

mkdir -p ../App/app/bin/
cp 7zip-download/7zz ../App/app/bin/
rm 7z2405-linux-x64.tar.xz
rm -rf 7zip-download