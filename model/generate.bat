@ECHO OFF
REM   Run all code generation scripts
REM   Requires iMatix GSL4, from http:\\www.nuget.org\packages\gslgen
REM   Use this when:
REM       - you add a socket option (sockopts.xml)
REM       - add a new project class (project.xml)
REM       - modify one of the referenced gsl templates (*.gsl)

mkdir ..\builds\mingw32
mkdir ..\builds\android
mkdir ..\builds\msvc\vs2008\czmq
mkdir ..\builds\msvc\vs2008\czmq_selftest
mkdir ..\builds\msvc\vs2010\czmq
mkdir ..\builds\msvc\vs2010\czmq_selftest
mkdir ..\builds\msvc\vs2012\czmq
mkdir ..\builds\msvc\vs2012\czmq_selftest
mkdir ..\builds\msvc\vs2013\czmq
mkdir ..\builds\msvc\vs2013\czmq_selftest

gslgen -q -script:sockopts.gsl sockopts.xml
gslgen -q -script:project.gsl project.xml
