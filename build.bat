:: build script with Qt-5 .pro

@echo on
@setlocal enabledelayedexpansion

@SET /a CORES=%NUMBER_OF_PROCESSORS%-1
@if %CORES% LSS 1 @SET CORES=1

@SET APPNAME=gamma
@SET BUILDTYPE=%1
@SET BUILDDIR=%1
@IF "%BUILDTYPE%" EQU "" (
    @SET BUILDTYPE=release
    @SET BUILDDIR=release
)
@SET EXEFILE=%BUILDDIR%\%APPNAME%.exe
@SET DISTDIR=dist

@echo build %EXEFILE% ...

@IF NOT EXIST "gamma.pro" (
    @echo.
    @echo ERROR: not found gamma.pro - directory not for building
    @echo.
    @pause
    @exit /b 1
)

@IF EXIST "%BUILDDIR%\" (
    @echo Remove current build directory - %BUILDDIR% ...
    @rd /Q /S %BUILDDIR%
    @if "%ERRORLEVEL%" NEQ "0" (
        @echo ERROR: Can't remove direcory %BUILDDIR%
        @echo.
        @pause
        @exit /b 1
    )
)

@echo Create build directory         - %BUILDDIR% ...
@mkdir %BUILDDIR%
@IF NOT EXIST "%BUILDDIR%\" (
    @echo ERROR: directory not found - %BUILDDIR%
    @echo.
    @pause
    @exit /b 1
)

mingw32-make.exe distclean 2>NUL
qmake.exe gamma.pro -spec win32-g++ "CONFIG+=%BUILDTYPE%"
@if "%ERRORLEVEL%" NEQ "0" (
    @echo.
    @echo ERROR: Can't create build configuration
    @echo.
    @pause
    @exit /b 1
)

mingw32-make.exe -j %CORES%
@if "%ERRORLEVEL%" NEQ "0" (
    @echo.
    @echo ERROR: build failed
    @echo.
    @pause
    @exit /b 1
)

@IF NOT EXIST "%EXEFILE%" (
    @echo.
    @echo ERROR: %EXEFILE% not found" ?
    @echo.
    @pause
    @exit /b 1
)

@rd /Q /S %DISTDIR%
@mkdir %DISTDIR%
mingw32-make.exe dist
@if "%ERRORLEVEL%" NEQ "0" (
    @echo.
    @echo ERROR: Failed to create production package - %DISTDIR%
    @echo.
    @pause
    @exit /b 1
)

@IF NOT EXIST "%DISTDIR%\" (
    @echo.
    @echo ERROR: production package not found - "%DISTDIR%" ?
    @echo.
    @pause
    @exit /b 1
)

@dir %DISTDIR%\

@rem -----------------------------------------------
@echo.
@echo The producation package built successfully
@echo.

@exit /b 0
