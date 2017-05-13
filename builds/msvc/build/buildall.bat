@ECHO OFF
:: Usage: buildall.bat

:: Build all configurations for all solutions.
@echo Building with VS2015
CALL ..\vs2015\build.bat
ECHO.
@echo Building with VS2013
CALL ..\vs2013\build.bat
ECHO.
@echo Building with VS2012
CALL ..\vs2012\build.bat
ECHO.
@echo Building with VS2010
CALL ..\vs2010\build.bat
ECHO.

PAUSE
