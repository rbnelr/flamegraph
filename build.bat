@echo off & setlocal enabledelayedexpansion
	
	cls
	color 07
	
	
rem		rem just for notebook for now
rem		set CPROJ=C:/Cproj/
rem		set MSVC_DIR=C:/Program Files (x86)/Microsoft Visual Studio/2017/
rem		
rem		call :vcvars
rem		goto :vcvars_end
rem		
rem	:vcvars
rem		!MSVC_DIR!Community/VC/Auxiliary/Build/vcvarsall.bat x86_amd64 >NUL
rem	:vcvars_end
rem		
rem		set path=!CPROJ!_mybin;!CPROJ!_llvm4.0/bin;!path!
rem		
rem		rem

	set CPROJ=D:/Cproj/
	
	set froot=%~dp0
	set fCproj=!CPROJ!
	set fsrc=!froot!src/
	
	set func=vs
	if not [%1] == []		set func=%1
	
	set release=0
	if [%2] == [dbg]		set release=0
	if [%2] == [release]	set release=1
	
	set project=graph
	if not [%3] == []		set project=%3
	
	set succ=0
	call :%func%
	
	if [succ] == [0] (
		echo fail.
	) else (
		echo success.
	)
	exit /b
rem /main

:vs
	if exist !froot!shaders\zz_*.tmp ( del !froot!shaders\zz_*.tmp )
	if exist !froot!!project!.pdb ( del !froot!!project!.pdb )
	if exist !froot!vc140.pdb ( del !froot!vc140.pdb )
	
	if [!release!] == [0] (
		set dbg=/Od /Oi /Zi /DEBUG /DDEBUG=1
	) else	(
		set dbg=/O2 /Oi /Zi /DEBUG /DDEBUG=0
	)
	
	set warn=/wd4577
	rem warning C4577 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed
	
	set opt=!dbg! /fp:fast /Ob2 /GR- /Gv /EHa- /Gy /GS-
	
	rem /showIncludes
	cl.exe -nologo /DNOTEBOOK=1 /D_PLATF=1 /D_ARCH=1 !opt! !warn! /I!fsrc!include /I!froot!../include !fsrc!!project!.cpp /Fe!froot!!project!.exe /link USER32.lib GDI32.lib OPENGL32.lib Ws2_32.lib /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /OPT:REF
	rem cl.exe /showIncludes -nologo /D_PLATF=1 /D_ARCH=1 !opt! !warn! /I!fCproj!/include !fsrc!!project!.cpp  -P
	
	exit /b
rem /a

:disasm
	dumpbin /ALL /DISASM /SYMBOLS /out:!froot!!project!.exe.asm.tmp !froot!!project!.exe
	undname !froot!!project!.exe.asm.tmp > !froot!!project!.exe.asm
	del !froot!!project!.exe.asm.tmp
	
	rem dumpbin /ALL /DISASM /SYMBOLS /out:!froot!!project!.exe.asm !froot!!project!.exe
	
	exit /b
rem /a

:dbg
	echo devenv !froot!!project!.exe
	devenv !froot!!project!.exe
	exit /b
rem /a

:llvm
	rem -g0 this still generates a .pdb! and is disasm'able with dumpbin but does source code steppable in visual studio
	if [!release!] == [0] (
		set dbg=-O0 -g0 -DDEBUG=1
	) else	(
		set dbg=-O3 -g0 -DDEBUG=0
	)
	
	rem set sdk=/LIBPATH:"!fCproj!\_winSDKOld\Lib\x64" /LIBPATH:"!fCproj!\_vs2015\VC\lib\amd64" /LIBPATH:"!fCproj!\_winSDKOld\Windows Kits\10\Lib\10.0.10150.0\ucrt\x64"
	set opt=!dbg! -fno-stack-protector -fno-math-errno -fno-exceptions -fno-unwind-tables -fno-strict-overflow -mmmx -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2 -mpopcnt
	
	rem set warn=-Wall -Wno-unused-variable -Wno-unused-function -Wno-tautological-compare -Wno-non-pod-varargs -Wno-invalid-offsetof
	set warn=-Wall -Wno-unused-variable -Wunused-function -Wno-tautological-compare -Wno-non-pod-varargs -Wno-invalid-offsetof
	rem -fmax-errors=5
	rem -Wno-unused-variable
	rem -Wno-unused-function
	rem -Wtautological-compare		constant if statements
	rem -Wno-invalid-offsetof		offsetof on non-POD structs (structs with inheritance in my case, i use inheritance for a simple enitity system, which does not use any virtual functions, nor will i ever use those, so this is OK)
	rem -fmax-errors=5

	set cmd=clang++ -std=c++11 -m64 -D_PLATF=1 -D_ARCH=1 !opt! !warn! -I!fsrc!/include -o !froot!!project!.exe !fsrc!!project!.cpp -Wl,/SUBSYSTEM:CONSOLE,/OPT:REF -lUSER32 -lGDI32 -lOPENGL32
	rem echo !cmd!
	!cmd!
	
	exit /b
rem /a



:gengl
	
	set count=0
	
	echo // This file was generated by 'build.bat genGLList'> !fsrc!\oglInterface_funcNames.hpp
	echo // This file was generated by 'build.bat genGLList'> !fsrc!\oglInterface_funcWrappers.hpp
	
	echo|set /p="#define OGL_FUNC_NAMES " >> !fsrc!\oglInterface_funcNames.hpp
	
	for /f "tokens=*" %%a in (!fsrc!\ogl_proc_list.txt) do (
		set a_temp=%%a
		set first_char=!a_temp:~0,1!
		if not [!first_char!] == [#] (
			
			cat !fsrc!/OpenGL32_dll_exports.txt | grep -xc %%a > exported_by_OpenGL32_dll.tmp
			set /p exported_by_OpenGL32_dll= < exported_by_OpenGL32_dll.tmp
			
			echo %%a| tr '[:lower:]' '[:upper:]' > fpType.tmp
			set /p fpType= < fpType.tmp
			set fpType=PFN!fpType!PROC
			
			if [!exported_by_OpenGL32_dll!] == [0] (
				
				echo|set /p="\">>!fsrc!\oglInterface_funcNames.hpp
				rem echo newline
				echo.>> !fsrc!\oglInterface_funcNames.hpp
				echo|set /p="X(%%a, !fpType!) ">> !fsrc!\oglInterface_funcNames.hpp
				
				echo #define %%a^(...^) CALL_OGL_FUNC_TBL^(%%a, __VA_ARGS__^)>> !fsrc!\oglInterface_funcWrappers.hpp
				
				set /a count+=1
				
				echo %%a	!fpType!
			) else (
				
				rem echo #define %%a^(...^) CALL_OGL_FUNC^(%%a, __VA_ARGS__^)>> !fsrc!\oglInterface_funcWrappers.hpp
				
				echo %%a	non-funcptr
			)
		)
	)
	rem echo newline
	echo.>> !fsrc!\oglInterface_funcNames.hpp
	
	echo // This file was generated by 'build.bat genGLList'> !fsrc!\oglInterface_funcCount.hpp
	echo !count!>> !fsrc!\oglInterface_funcCount.hpp
	
	del !froot!\*.tmp
	
	echo -genGLList requires full rebuild, exe to make your life easier
	del !froot!!project!.exe
	
	exit /b
rem /a

