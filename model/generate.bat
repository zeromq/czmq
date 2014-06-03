@ECHO OFF
REM   Rebuild project files
REM   Requires iMatix GSL4, from http:\\www.nuget.org\packages\gsl
REM   Use this when:
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

gsl -q -script:project.gsl project.xml
