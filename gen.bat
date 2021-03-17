@echo off

if not exist build mkdir build
pushd build
cl /Zi /FC /nologo ..\generator\static_site_generator.c
popd

if not exist generated mkdir generated
pushd generated
..\build\static_site_generator.exe --siteinfo ..\site_info.md --pagedir ..\pages\
xcopy ..\data .\data\ /y /s /e /q >NUL
popd