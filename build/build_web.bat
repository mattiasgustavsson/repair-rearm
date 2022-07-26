@echo off
REM make sure 'wasm' folder of the WebAssembly build environment is placed in the parent folder of this script
REM it can be downloaded from https://github.com/mattiasgustavsson/dos-like/releases/tag/wasm-env
REM also, the repair-rearm.exe needs to be in the parent folder of this script, so be sure to run build_win.bat first
pushd %~dp0
pushd ..
repair-rearm --build
popd
..\wasm\node ..\wasm\wajicup.js -embed repair-rearm.dat ../repair-rearm.dat -template template.html -rle ../source/repair-rearm.c ..\repair-rearm.html
popd