@ECHO OFF

CALL buildbase.bat ..\vs2013\czmq.sln 12
ECHO.
CALL buildbase.bat ..\vs2012\czmq.sln 11
ECHO.
CALL buildbase.bat ..\vs2010\czmq.sln 10
ECHO.

PAUSE