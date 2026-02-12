@echo off
REM Script para compilar TODOS los programas del proyecto
REM Compila: db/ (SQL), serial/, arduino/, y main.c

setlocal enabledelayedexpansion

echo.
echo ========================================
echo   Compilar TODOS los programas
echo ========================================
echo.

set "COMPILED=0"
set "FAILED=0"

REM Compilar programas SQL (db/)
echo [1/5] Compilando db\test_sql.c...
gcc -g db\test_sql.c db\sql_connection.c -o db\test_sql.exe -lodbc32
if %ERRORLEVEL% EQU 0 (
    echo       ✓ db\test_sql.exe compilado
    set /a COMPILED+=1
) else (
    echo       ✗ Error en db\test_sql.c
    set /a FAILED+=1
)
echo.

echo [2/5] Compilando db\menu_tabla.c...
gcc -g db\menu_tabla.c db\sql_connection.c -o db\menu_tabla.exe -lodbc32
if %ERRORLEVEL% EQU 0 (
    echo       ✓ db\menu_tabla.exe compilado
    set /a COMPILED+=1
) else (
    echo       ✗ Error en db\menu_tabla.c
    set /a FAILED+=1
)
echo.

REM Compilar programas serial
echo [3/5] Compilando serial\serial.c...
gcc -g serial\serial.c -o serial\serial.exe
if %ERRORLEVEL% EQU 0 (
    echo       ✓ serial\serial.exe compilado
    set /a COMPILED+=1
) else (
    echo       ✗ Error en serial\serial.c
    set /a FAILED+=1
)
echo.

REM Compilar arduino
echo [4/5] Compilando arduino\arduino.c...
gcc -g arduino\arduino.c -o arduino\arduino.exe
if %ERRORLEVEL% EQU 0 (
    echo       ✓ arduino\arduino.exe compilado
    set /a COMPILED+=1
) else (
    echo       ✗ Error en arduino\arduino.c
    set /a FAILED+=1
)
echo.

REM Compilar main.c
echo [5/5] Compilando main.c...
gcc -g main.c -o main.exe
if %ERRORLEVEL% EQU 0 (
    echo       ✓ main.exe compilado
    set /a COMPILED+=1
) else (
    echo       ✗ Error en main.c
    set /a FAILED+=1
)
echo.

REM Resumen
echo ========================================
echo   Resumen de Compilación
echo ========================================
echo   Exitosos: %COMPILED%
echo   Fallidos:  %FAILED%
echo ========================================
echo.

pause
endlocal
