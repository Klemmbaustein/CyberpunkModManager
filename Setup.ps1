# Dependencies/7zip/CPP/7zip
cd Dependencies/7zip/CPP/7zip

# 7zip is insane.
if ($args[0] -ne "CI_BUILD")
{
	nmake
}
mkdir ../../../../App/app/bin -f
cp UI/Console/o/7z.exe ../../../../App/app/bin/
cp Bundles/Format7zF/o/7z.dll ../../../../App/app/bin/

# ./
cd ../../../..
