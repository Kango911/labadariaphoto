#ifndef IMAGE_H
#define IMAGE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Структура для представления цвета (RGB)
typedef struct {
    float r, g, b;
} Color;

// Структура для представления изображения
typedef struct {
    Color* data;
    int width;
    int height;
    int capacity; // Для динамического массива
} Image;

// Создание и уничтожение изображения
Image* image_create(int width, int height);
void image_destroy(Image* image);

// Копирование изображения
Image* image_copy(const Image* src);

// Получение и установка пикселей
Color image_get_pixel(const Image* image, int x, int y);
void image_set_pixel(Image* image, int x, int y, Color color);

// Проверка границ
bool image_is_valid_coord(const Image* image, int x, int y);

// Изменение размера
void image_resize(Image* image, int new_width, int new_height);

// Вспомогательные функции для работы с цветом
Color color_create(float r, float g, float b);
Color color_add(Color c1, Color c2);
Color color_sub(Color c1, Color c2);
Color color_mul(Color c, float scalar);
Color color_clamp(Color c);
float color_luminance(Color c);

#endif // IMAGE_H