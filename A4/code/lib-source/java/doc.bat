@echo off

if %JDK_HOME%.==. goto SETHOME

del /Q docs\*.*
del /Q docs\java\lang\*.*
del /Q docs\java\io\*.*
del /Q docs\java\util\*.*

REM For jdk1.4 and higher add -breakiterator command line switch
%JDK_HOME%\bin\javadoc -d docs -bootclasspath ..\ -sourcepath ..\ java.lang java.io java.util
echo Done.
goto EXIT

:SETHOME
echo Missing JDK_HOME environment variable. For example: SET JDK_HOME=C:\JDK1.3

:EXIT
pause