@echo off
:: Give some time for the installer to exit after it has called the update script.
ping 127.0.0.1 -n 2  nul
:: Copy the new app version into the current directory.
xcopy /s/e/y app\temp\update .\
:: Start the mod manager again
start CyberpunkModManager

