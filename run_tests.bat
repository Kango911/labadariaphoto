@echo off
chcp 65001 > nul
echo Тестирование ImageCraft...
echo.

REM Проверка наличия исполняемого файла
if not exist "image_craft.exe" (
    echo Ошибка: image_craft.exe не найден!
    echo Запустите build.bat для сборки.
    pause
    exit /b 1
)

REM Создание тестового BMP, если нет lenna.bmp
if not exist "tests\test_images\lenna.bmp" (
    echo Создание тестового изображения...
    python -c "
import struct
# Создаем простой BMP 64x64
width = 64
height = 64
row_padding = (4 - (width * 3) % 4) % 4
row_size = width * 3 + row_padding
image_size = row_size * height
file_size = 54 + image_size

with open('tests\\test_images\\test_pattern.bmp', 'wb') as f:
    # Заголовок файла
    f.write(b'BM')  # signature
    f.write(struct.pack('<I', file_size))  # file size
    f.write(struct.pack('<I', 0))  # reserved
    f.write(struct.pack('<I', 54))  # data offset
    
    # Информационный заголовок
    f.write(struct.pack('<I', 40))  # header size
    f.write(struct.pack('<i', width))  # width
    f.write(struct.pack('<i', height))  # height
    f.write(struct.pack('<H', 1))  # planes
    f.write(struct.pack('<H', 24))  # bits per pixel
    f.write(struct.pack('<I', 0))  # compression
    f.write(struct.pack('<I', image_size))  # image size
    f.write(struct.pack('<i', 2835))  # x pixels per meter
    f.write(struct.pack('<i', 2835))  # y pixels per meter
    f.write(struct.pack('<I', 0))  # colors used
    f.write(struct.pack('<I', 0))  # important colors
    
    # Данные изображения (шахматная доска)
    for y in range(height):
        for x in range(width):
            if (x // 8 + y // 8) % 2 == 0:
                # Белый
                f.write(struct.pack('BBB', 255, 255, 255))
            else:
                # Черный
                f.write(struct.pack('BBB', 0, 0, 0))
        # Выравнивание
        f.write(b'\x00' * row_padding)
" 2>nul
    if exist "tests\test_images\test_pattern.bmp" (
        echo Тестовое изображение создано: tests\test_images\test_pattern.bmp
        set TEST_IMAGE=test_pattern.bmp
    ) else (
        echo Предупреждение: Не удалось создать тестовое изображение.
        echo Поместите любое BMP в tests\test_images\
        pause
        exit /b 1
    )
) else (
    set TEST_IMAGE=lenna.bmp
)

echo Запуск тестов...
echo.

REM Тест 1: Градации серого
echo Тест 1: Градации серого (-gs)
image_craft.exe tests\test_images\%TEST_IMAGE% tests\output_gs.bmp -gs
if exist "tests\output_gs.bmp" (
    echo ✅ Успешно: tests\output_gs.bmp
) else (
    echo ❌ Ошибка
)

REM Тест 2: Негатив
echo.
echo Тест 2: Негатив (-neg)
image_craft.exe tests\test_images\%TEST_IMAGE% tests\output_neg.bmp -neg
if exist "tests\output_neg.bmp" (
    echo ✅ Успешно: tests\output_neg.bmp
) else (
    echo ❌ Ошибка
)

REM Тест 3: Обрезка
echo.
echo Тест 3: Обрезка (-crop)
image_craft.exe tests\test_images\%TEST_IMAGE% tests\output_crop.bmp -crop 32 32
if exist "tests\output_crop.bmp" (
    echo ✅ Успешно: tests\output_crop.bmp
) else (
    echo ❌ Ошибка
)

REM Тест 4: Комбинированный
echo.
echo Тест 4: Комбинированный (-gs -neg)
image_craft.exe tests\test_images\%TEST_IMAGE% tests\output_combined.bmp -gs -neg
if exist "tests\output_combined.bmp" (
    echo ✅ Успешно: tests\output_combined.bmp
) else (
    echo ❌ Ошибка
)

echo.
echo Все тесты завершены!
echo Проверьте файлы в папке tests\
pause