@echo off

set config=Release
set outputdir=%cwd%\build
set cwd=%CD%
set commonflags=/p:Configuration=%config% /p:CLSCompliant=True 

set fdir=%WINDIR%\Microsoft.NET\Framework
set msbuild=%fdir%\v4.0.30319\msbuild.exe

:build
echo ---------------------------------------------------------------------
echo Building AnyCpu release...
rem %msbuild% MetroFramework\MetroFramework.csproj %commonflags% /tv:2.0 /p:TargetFrameworkVersion=v2.0 /p:Platform="Any Cpu" /p:OutputPath="%outputdir%\AnyCpu\NET20"
rem if errorlevel 1 goto build-error
rem %msbuild% MetroFramework\MetroFramework.csproj %commonflags% /tv:3.5 /p:TargetFrameworkVersion=v3.5 /p:Platform="Any Cpu" /p:OutputPath="%outputdir%\AnyCpu\NET35"
rem if errorlevel 1 goto build-error
rem %msbuild% MetroFramework\MetroFramework.csproj %commonflags% /tv:4.0 /p:TargetFrameworkVersion=v4.0 /p:Platform="Any Cpu" /p:OutputPath="%outputdir%\AnyCpu\NET40"
rem if errorlevel 1 goto build-error
rem %msbuild% MetroFramework\MetroFramework.csproj %commonflags% /tv:4.0 /p:TargetFrameworkVersion=v4.5 /p:Platform="Any Cpu" /p:OutputPath="%outputdir%\AnyCpu\NET45"
rem if errorlevel 1 goto build-error

%msbuild% MetroFramework.sln %commonflags% /tv:4.0 /p:TargetFrameworkVersion=v4.0 /p:Platform="Any Cpu"
if errorlevel 1 goto build-error

nuget pack MetroFramework\MetroFramework.csproj -Prop Configuration=Release
if errorlevel 1 goto build-error
nuget pack MetroFramework.Design\MetroFramework.Design.csproj -Prop Configuration=Release
if errorlevel 1 goto build-error
nuget pack MetroFramework.Fonts\MetroFramework.Fonts.csproj -Prop Configuration=Release
if errorlevel 1 goto build-error
nuget pack NuGet\MetroFramework.nuspec
if errorlevel 1 goto build-error


:build-error
echo Failed to compile...

:done
echo.
echo ---------------------------------------------------------------------
echo Compile finished....
cd %cwd%
goto exit

:exit