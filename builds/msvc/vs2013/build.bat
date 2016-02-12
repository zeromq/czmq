@ECHO OFF
SET solution=czmq.sln
SET version=12
SET log=build.log
SET tools=Microsoft Visual Studio %version%.0\VC\vcvarsall.bat
SET environment="%programfiles(x86)%\%tools%"
IF NOT EXIST %environment% SET environment="%programfiles%\%tools%"
IF NOT EXIST %environment% GOTO no_tools

SET packages=
IF EXIST "..\..\..\..\libzmq\builds/msvc/vs2013\libzmq.import.props" (
    COPY /Y "..\..\..\..\libzmq\builds/msvc/vs2013\libzmq.import.props" . > %log%
    IF errorlevel 1 GOTO error
) ELSE (
    ECHO Did not find libzmq, aborting.
    ECHO Please clone from https://github.com/zeromq/libzmq, and then build.
    GOTO error
)
IF EXIST "..\..\..\..\uuid\builds/msvc/vs2013\uuid.import.props" (
    COPY /Y "..\..\..\..\uuid\builds/msvc/vs2013\uuid.import.props" . > %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_UUID=1
    ECHO Building with uuid
) ELSE (
    ECHO Building without uuid
)
IF EXIST "..\..\..\..\systemd\builds/msvc/vs2013\systemd.import.props" (
    COPY /Y "..\..\..\..\systemd\builds/msvc/vs2013\systemd.import.props" . > %log%
    IF errorlevel 1 GOTO error
    SET packages=%packages% /p:HAVE_SYSTEMD=1
    ECHO Building with systemd
) ELSE (
    ECHO Building without systemd
)

ECHO Building CZMQ... (%packages%)

CALL %environment% x86 > nul
ECHO Platform=x86

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=Win32 %packages% %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=Win32 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error

CALL %environment% x86_amd64 > nul
ECHO Platform=x64

ECHO Configuration=DynDebug
msbuild /m /v:n /p:Configuration=DynDebug /p:Platform=x64 %packages% %solution% > %log%
IF errorlevel 1 GOTO error
ECHO Configuration=DynRelease
msbuild /m /v:n /p:Configuration=DynRelease /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgDebug
msbuild /m /v:n /p:Configuration=LtcgDebug /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=LtcgRelease
msbuild /m /v:n /p:Configuration=LtcgRelease /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticDebug
msbuild /m /v:n /p:Configuration=StaticDebug /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error
ECHO Configuration=StaticRelease
msbuild /m /v:n /p:Configuration=StaticRelease /p:Platform=x64 %packages% %solution% >> %log%
IF errorlevel 1 GOTO error

ECHO Complete: %packages% %solution%
GOTO end

:error
ECHO *** ERROR, build terminated early: see %log%
GOTO end

:no_tools
ECHO *** ERROR, build tools not found: %tools%

:end
