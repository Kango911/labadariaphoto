@echo off
echo Быстрая компиляция ImageCraft...
gcc -std=c11 -Wall -Wextra -O2 -D_CRT_SECURE_NO_WARNINGS ^
    src\main.c src\image.c src\bmp.c src\filters.c src\pipeline.c src\cli.c ^
    -o image_craft.exe -lm

if %errorlevel% equ 0 (
    echo Успешно! image_craft.exe создан.
) else (
    echo Ошибка компиляции.
)
pause