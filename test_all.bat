@echo off
echo Тестирование всех фильтров...
echo.

REM 1. Градации серого
image_craft.exe tests\test_images\lenna.bmp output_1_gs.bmp -gs
echo Создан: output_1_gs.bmp

REM 2. Негатив
image_craft.exe tests\test_images\lenna.bmp output_2_neg.bmp -neg
echo Создан: output_2_neg.bmp

REM 3. Обрезка
image_craft.exe tests\test_images\lenna.bmp output_3_crop.bmp -crop 150 150
echo Создан: output_3_crop.bmp

REM 4. Повышение резкости
image_craft.exe tests\test_images\lenna.bmp output_4_sharp.bmp -sharp
echo Создан: output_4_sharp.bmp

REM 5. Обнаружение границ
image_craft.exe tests\test_images\lenna.bmp output_5_edge.bmp -edge 0.1
echo Создан: output_5_edge.bmp

REM 6. Медианный фильтр
image_craft.exe tests\test_images\lenna.bmp output_6_med.bmp -med 3
echo Создан: output_6_med.bmp

REM 7. Размытие
image_craft.exe tests\test_images\lenna.bmp output_7_blur.bmp -blur 0.5
echo Создан: output_7_blur.bmp

REM 8. Сепия
image_craft.exe tests\test_images\lenna.bmp output_8_sepia.bmp -sepia
echo Создан: output_8_sepia.bmp

REM 9. Виньетирование
image_craft.exe tests\test_images\lenna.bmp output_9_vignette.bmp -vignette 0.7
echo Создан: output_9_vignette.bmp

echo.
echo Все тестовые изображения созданы!
dir output_*.bmp
pause