@echo off
REM run this from a MSVC Developer Command Prompt
pushd %~dp0
rc /nologo repair_rearm.rc"
cl "..\source\repair-rearm.c" repair_rearm.res /nologo /O2 /Ob2 /Oi /Ot /Oy /MT /GL /GF /D "NDEBUG" /Fe:"..\repair-rearm.exe" /link /INCREMENTAL:NO /OPT:REF /OPT:ICF /LTCG /SUBSYSTEM:WINDOWS 
pushd ..
repair-rearm --build
popd
popd