@echo off
REM Script para limpiar archivos compilados de todas las carpetas

echo.
echo Limpiando archivos compilados (.exe)...
echo.

set "DELETED=0"

REM Limpiar archivos SQL (db/)
if exist db\test_sql.exe (
    del db\test_sql.exe
    echo ✓ db\test_sql.exe eliminado
    set /a DELETED+=1
)

if exist db\menu_tabla.exe (
    del db\menu_tabla.exe
    echo ✓ db\menu_tabla.exe eliminado
    set /a DELETED+=1
)

if exist db\ejemplo_sql.exe (
    del db\ejemplo_sql.exe
    echo ✓ db\ejemplo_sql.exe eliminado
    set /a DELETED+=1
)

REM Limpiar archivos serial
if exist serial\serial.exe (
    del serial\serial.exe
    echo ✓ serial\serial.exe eliminado
    set /a DELETED+=1
)

REM Limpiar archivos arduino
if exist arduino\arduino.exe (
    del arduino\arduino.exe
    echo ✓ arduino\arduino.exe eliminado
    set /a DELETED+=1
)

REM Limpiar main.exe (raíz)
if exist main.exe (
    del main.exe
    echo ✓ main.exe eliminado
    set /a DELETED+=1
)

if %DELETED%==0 (
    echo No hay archivos que limpiar
) else (
    echo.
    echo Total eliminados: %DELETED%
)

echo.
pause
