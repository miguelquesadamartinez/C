@echo off
REM Compila y ejecuta un programa en un solo paso
REM Uso: run.bat [ruta\archivo.c]
REM Ejemplos: run.bat db\test_sql.c
REM           run.bat serial\serial.c
REM           run.bat (ejecuta db\test_sql.c por defecto)

setlocal

if "%~1"=="" (
    set "SOURCE=db\test_sql.c"
    set "OUTPUT=db\test_sql.exe"
) else (
    set "SOURCE=%~1"
    set "OUTPUT=%~n1.exe"
)

echo.
echo Compilando %SOURCE%...

REM Detectar si es un archivo SQL (necesita sql_connection.c)
echo %SOURCE% | findstr /i "db\\" >nul
if %ERRORLEVEL% EQU 0 (
    gcc -g "%SOURCE%" db\sql_connection.c -o "%OUTPUT%" -lodbc32
) else (
    gcc -g "%SOURCE%" -o "%OUTPUT%"
)

if %ERRORLEVEL% EQU 0 (
    echo ✓ Compilado
    echo.
    echo ========================================
    echo   Ejecutando %OUTPUT%
    echo ========================================
    echo.
    "%OUTPUT%"
) else (
    echo ✗ Error en la compilación
    echo.
    pause
    exit /b 1
)

echo.
pause
endlocal
