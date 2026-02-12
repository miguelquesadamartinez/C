@echo off
REM Script de compilación para programas SQL en C
REM Uso: build.bat [ruta\archivo.c]
REM Ejemplos: build.bat db\test_sql.c
REM           build.bat (compila db\test_sql.c por defecto)

setlocal

REM Colores (opcional, pero mejora la visualización)
set "SUCCESS=[32m"
set "ERROR=[31m"
set "INFO=[36m"
set "RESET=[0m"

echo.
echo ========================================
echo   Compilador de Programas C
echo ========================================
echo.

REM Verificar si se pasó un argumento
if "%~1"=="" (
    set "SOURCE=db\test_sql.c"
    set "OUTPUT=db\test_sql.exe"
) else (
    set "SOURCE=%~1"
    set "OUTPUT=%~n1.exe"
)

REM Verificar que el archivo fuente existe
if not exist "%SOURCE%" (
    echo %ERROR%Error: El archivo %SOURCE% no existe%RESET%
    echo.
    pause
    exit /b 1
)

REM Compilar (siempre incluye sql_connection.c desde db/)
echo %INFO%Compilando %SOURCE%...%RESET%
echo.
gcc -g "%SOURCE%" db\sql_connection.c -o "%OUTPUT%" -lodbc32

REM Verificar si la compilación fue exitosa
if %ERRORLEVEL% EQU 0 (
    echo.
    echo %SUCCESS%✓ Compilación exitosa!%RESET%
    echo   Ejecutable: %OUTPUT%
    echo.
    
    REM Preguntar si desea ejecutar
    set /p RUN="¿Deseas ejecutar el programa? (S/N): "
    if /i "!RUN!"=="S" (
        echo.
        echo ========================================
        echo   Ejecutando %OUTPUT%
        echo ========================================
        echo.
        "%OUTPUT%"
    )
) else (
    echo.
    echo %ERROR%✗ Error en la compilación%RESET%
    echo.
    pause
    exit /b 1
)

echo.
echo ========================================
echo   Proceso finalizado
echo ========================================
echo.

pause
endlocal
