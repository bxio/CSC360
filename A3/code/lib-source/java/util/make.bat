@echo off

if %JDK_HOME%.==. goto SETHOME

if %1.==. goto ALLFILES

%JDK_HOME%\bin\javac -target 1.1 -classpath . -bootclasspath .;..\.. %1.java
if errorlevel==1 goto ERROR
goto OKMSG

:ALLFILES
%JDK_HOME%\bin\javac -target 1.1 -classpath . -bootclasspath .;..\.. *.java
if errorlevel==1 goto ERROR

:OKMSG
echo Process completed successully
goto EXIT

:ERROR
echo Compilation error
goto EXIT

:SETHOME
echo Missing JDK_HOME environment variable. For example: SET JDK_HOME=C:\JDK1.3

:EXIT



