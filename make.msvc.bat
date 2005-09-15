@ECHO OFF


:: Toolkit 2003
if defined VCToolkitInstallDir (
	if exist "%VCToolkitInstallDir%\vcvars32.bat2" (
		echo -
		echo - VC 7.1 Toolkit found.
		echo -
		call "%VCToolkitInstallDir%\vcvars32.bat"
		goto STARTCOMPILING
	)
)

:: Viual C++
if defined VCINSTALLDIR (
	if exist "%VCINSTALLDIR%\vcvarsall.bat" (
		echo -
		echo - Visual C++ found.
		echo -
		call "%VCINSTALLDIR%\vcvarsall.bat"
		goto STARTCOMPILING
	)
	if exist "%VCINSTALLDIR%\vcvars32.bat" (
		echo -
		echo - Visual C++ found.
		echo -
		call "%VCINSTALLDIR%\vcvars32.bat"
		goto STARTCOMPILING
	)
)

echo -
echo - No Visual C++ found, please set the enviroment variable 
echo - 
echo - VCToolkitInstallDir  or  VCINSTALLDIR 
echo - 
echo - to your Visual C++ installation folder.
echo - 
echo - Or call the vcvars32.bat/vcvarsall.bat file.
echo -

goto ERROR


:STARTCOMPILING

:: loki buid process

@ECHO ON

cd src
call make.msvc.bat
cd ..

cd examples
call make.msvc.bat
cd ..

cd test\flex_string
call make.msvc.bat
cd ..\..

cd test\SafeFormat
call make.msvc.bat
cd ..\..

cd test\SmallObj
call make.msvc.bat
cd ..\..

cd tools\RegressionTest
call make.msvc.bat
cd ..\..

goto LEAVE





:ERROR
echo -
echo -
echo - An error occured. Compiling aborted.
echo - 
pause

:LEAVE