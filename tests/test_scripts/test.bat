@echo off
chcp 65001 > nul
echo Тестирование ImageCraft...
echo.

REM Проверка наличия исполняемого файла
cd ..
if not exist "image_craft.exe" (
    echo Ошибка: image_craft.exe не найден!
    echo Запустите build.bat для сборки.
    pause
    exit /b 1
)

REM Создание тестового изображения, если его нет
if not exist "tests\test_images\test.bmp" (
    echo Создание тестового изображения...

    REM Создаем простой BMP через Python
    python -c "
import struct

width = 64
height = 64
row_padding = (4 - (width * 3) % 4) % 4
row_size = width * 3 + row_padding
image_size = row_size * height
file_size = 54 + image_size

with open('tests\\test_images\\test.bmp', 'wb') as f:
    # Заголовок файла
    f.write(b'BM')
    f.write(struct.pack('<I', file_size))
    f.write(struct.pack('<I', 0))
    f.write(struct.pack('<I', 54))

    # Информационный заголовок
    f.write(struct.pack('<I', 40))
    f.write(struct.pack('<i', width))
    f.write(struct.pack('<i', height))
    f.write(struct.pack('<H', 1))
    f.write(struct.pack('<H', 24))
    f.write(struct.pack('<I', 0))
    f.write(struct.pack('<I', image_size))
    f.write(struct.pack('<i', 2835))
    f.write(struct.pack('<i', 2835))
    f.write(struct.pack('<I', 0))
    f.write(struct.pack('<I', 0))

    # Данные изображения (градиент)
    for y in range(height):
        for x in range(width):
            r = int(x / width * 255)
            g = int(y / height * 255)
            b = int((x + y) / (width + height) * 255)
            f.write(struct.pack('BBB', b, g, r))
        f.write(b'\x00' * row_padding)

print('Тестовое изображение создано')
" 2>nul
)

echo Запуск тестовых сценариев...
echo.

REM Тест 1: Градации серого
echo [Тест 1] Градации серого
image_craft.exe tests\test_images\test.bmp tests\output_gs.bmp -gs
if %errorlevel% equ 0 (
    echo ✅ Успешно
) else (
    echo ❌ Ошибка
)

REM Тест 2: Негатив
echo.
echo [Тест 2] Негатив
image_craft.exe tests\test_images\test.bmp tests\output_neg.bmp -neg
if %errorlevel% equ 0 (
    echo ✅ Успешно
) else (
    echo ❌ Ошибка
)

REM Тест 3: Обрезка
echo.
echo [Тест 3] Обрезка
image_craft.exe tests\test_images\test.bmp tests\output_crop.bmp -crop 32 32
if %errorlevel% equ 0 (
    echo ✅ Успешно
) else (
    echo ❌ Ошибка
)

REM Тест 4: Комбинированный
echo.
echo [Тест 4] Комбинированный (градации серого + негатив)
image_craft.exe tests\test_images\test.bmp tests\output_combined.bmp -gs -neg
if %errorlevel% equ 0 (
    echo ✅ Успешно
) else (
    echo ❌ Ошибка
)

REM Тест 5: Размытие
echo.
echo [Тест 5] Гауссово размытие
image_craft.exe tests\test_images\test.bmp tests\output_blur.bmp -blur 0.5
if %errorlevel% equ 0 (
    echo ✅ Успешно
) else (
    echo ❌ Ошибка
)

REM Тест 6: Сепия
echo.
echo [Тест 6] Сепия
image_craft.exe tests\test_images\test.bmp tests\output_sepia.bmp -sepia
if %errorlevel% equ 0 (
    echo ✅ Успешно
) else (
    echo ❌ Ошибка
)

echo.
echo ========================================
echo Тестирование завершено!
echo Проверьте файлы в папке tests\
echo.
dir tests\output_*.bmp
echo.
pause