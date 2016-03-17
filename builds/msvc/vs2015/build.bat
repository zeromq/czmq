@ECHO OFF
:: Usage:     build.bat [Clean]
@setlocal

:: supports passing in Clean as third argument if "make clean" behavior is desired
SET target=%1
if NOT "%target%" == "" set target=/t:%target%


SET solution=czmq.sln
SET version=14
SET vsversion=15
SET log=build.log
SET tools=Microsoft Visual Studio %version%.0\VC\vcvarsall.bat
SET environment="%programfiles(x86)%\%tools%"
IF NOT EXIST %environment% SET environment="%programfiles%\%tools%"
IF NOT EXIST %environment% GOTO no_tools

SET packages=
IF EXIST "..\..\..\..\libzmq\builds/msvc/vs20%vsversion%\libzmq.import.props" (
    COPY /Y "..\..\..\..\libzmq\builds/msvc/vs20%vsversion%\libzmq.import.props" .  >> %log%
    IF errorlevel 1 GOTO error
) ELSE (
    ECHO Did not find libzmq, aborting.
    ECHO Please clone from https://github.com/zeromq/libzmq, and then build.
    GOTO error
)
IF EXIST "..\..\..\..\uuid\builds/msvc/vs20%vsversion%\uuid.import.props" (
    COPY /Y "..\..\..\..\uuid\builds/msvc/vs20%vsversion%\uuid.import.props" .  >> %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_UUID=1
    ECHO Building with uuid
) ELSE (
    ECHO Building without uuid
)
IF EXIST "..\..\..\..\systemd\builds/msvc/vs20%vsversion%\systemd.import.props" (
    COPY /Y "..\..\..\..\systemd\builds/msvc/vs20%vsversion%\systemd.import.props" .  >> %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_SYSTEMD=1
    ECHO Building with systemd
) ELSE (
    ECHO Building without systemd
)

ECHO Building CZMQ... (%packages%)

set oldpath=%PATH%
CALL %environment% x86 >> %log%
ECHO Platform=x86

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=Win32 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=Win32 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=Win32 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=Win32 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=Win32 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=Win32 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
set PATH=%oldpath%

CALL %environment% x86_amd64  >> %log%
ECHO Platform=x64

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=x64 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=x64 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=x64 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=x64 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=x64 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=x64 %packages% %solution% %target% >> %log%
IF errorlevel 1 GOTO error
set PATH=%oldpath%
set oldpath=

ECHO Complete: %packages% %solution%
GOTO end

:error
if NOT "%oldpath%" == "" set PATH=%oldpath%&set oldpath=
ECHO *** ERROR, build terminated early: see %log%
GOTO end

:no_tools
ECHO *** ERROR, build tools not found: %tools%

:end
@endlocal
