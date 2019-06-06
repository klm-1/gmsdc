del Build.zip
rmdir /S /Q Build
mkdir Build
mkdir Build\out
copy /Y bin\Debug\disasm1.exe Build\gmsdc.exe
copy /Y binary_fmt.zip Build
copy /Y data.win Build
xcopy /E /Y "D:\GM\Projects\test\binary_fmt.gmx" Build\binary_fmt.gmx\
7z a -r Build.zip Build