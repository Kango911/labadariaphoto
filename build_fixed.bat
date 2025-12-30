@echo off
chcp 65001 > nul
echo ===========================================
echo   ImageCraft - Лабораторная работа №1
echo ===========================================
echo.

REM Проверка наличия компилятора
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ ОШИБКА: GCC не найден!
    echo.
    echo Установите MinGW:
    echo 1. Скачайте с https://sourceforge.net/projects/mingw/
    echo 2. Установите пакеты: mingw32-base, mingw32-gcc-g++, mingw32-make
    echo 3. Добавьте C:\MinGW\bin в переменную PATH
    echo 4. Перезапустите терминал
    echo.
    pause
    exit /b 1
)

echo ✅ GCC найден
echo.

REM Создание папок
if not exist "tests\test_images" mkdir tests\test_images
if not exist "tests\test_scripts" mkdir tests\test_scripts

REM Компиляция с отключением предупреждения о неиспользуемых параметрах
echo 🔧 Компиляция проекта...
echo.

gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -O2 -D_CRT_SECURE_NO_WARNINGS -c src\main.c -o main.o
if %errorlevel% neq 0 goto error

gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -O2 -D_CRT_SECURE_NO_WARNINGS -c src\image.c -o image.o
if %errorlevel% neq 0 goto error

gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -O2 -D_CRT_SECURE_NO_WARNINGS -c src\bmp.c -o bmp.o
if %errorlevel% neq 0 goto error

gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -O2 -D_CRT_SECURE_NO_WARNINGS -c src\filters.c -o filters.o
if %errorlevel% neq 0 goto error

gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -O2 -D_CRT_SECURE_NO_WARNINGS -c src\pipeline.c -o pipeline.o
if %errorlevel% neq 0 goto error

gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -O2 -D_CRT_SECURE_NO_WARNINGS -c src\cli.c -o cli.o
if %errorlevel% neq 0 goto error

echo.
echo 🔗 Линковка...
gcc main.o image.o bmp.o filters.o pipeline.o cli.o -o image_craft.exe -lm
if %errorlevel% neq 0 goto error

REM Очистка временных файлов
del *.o 2>nul

echo.
echo ✅ СБОРКА ЗАВЕРШЕНА!
echo.
echo Исполняемый файл: image_craft.exe
echo.
echo Примеры использования:
echo   image_craft.exe input.bmp output.bmp -gs
echo   image_craft.exe input.bmp output.bmp -crop 100 100 -neg
echo   image_craft.exe input.bmp output.bmp -blur 0.5 -sharp
echo.
echo Для помощи: image_craft.exe -h
echo.
goto end

:error
echo.
echo ❌ ОШИБКА КОМПИЛЯЦИИ!
echo Проверьте исходные файлы.
pause
exit /b 1

:end
pause