#include "image.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

Image* image_create(int width, int height) {
    if (width <= 0 || height <= 0) {
        return NULL;
    }

    Image* image = (Image*)malloc(sizeof(Image));
    if (!image) {
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->capacity = width * height;

    image->data = (Color*)malloc(sizeof(Color) * image->capacity);
    if (!image->data) {
        free(image);
        return NULL;
    }

    // Инициализация черным цветом
    memset(image->data, 0, sizeof(Color) * image->capacity);

    return image;
}

void image_destroy(Image* image) {
    if (image) {
        free(image->data);
        free(image);
    }
}

Image* image_copy(const Image* src) {
    if (!src) {
        return NULL;
    }

    Image* dst = image_create(src->width, src->height);
    if (!dst) {
        return NULL;
    }

    memcpy(dst->data, src->data, sizeof(Color) * src->width * src->height);
    return dst;
}

Color image_get_pixel(const Image* image, int x, int y) {
    if (!image || !image_is_valid_coord(image, x, y)) {
        return color_create(0, 0, 0);
    }

    return image->data[y * image->width + x];
}

void image_set_pixel(Image* image, int x, int y, Color color) {
    if (!image || !image_is_valid_coord(image, x, y)) {
        return;
    }

    image->data[y * image->width + x] = color_clamp(color);
}

bool image_is_valid_coord(const Image* image, int x, int y) {
    return image && x >= 0 && x < image->width && y >= 0 && y < image->height;
}

void image_resize(Image* image, int new_width, int new_height) {
    if (!image || new_width <= 0 || new_height <= 0) {
        return;
    }

    Color* new_data = (Color*)malloc(sizeof(Color) * new_width * new_height);
    if (!new_data) {
        return;
    }

    // Копирование с масштабированием (простое ближайшее соседство)
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            int src_x = x * image->width / new_width;
            int src_y = y * image->height / new_height;
            src_x = src_x < image->width ? src_x : image->width - 1;
            src_y = src_y < image->height ? src_y : image->height - 1;

            new_data[y * new_width + x] = image_get_pixel(image, src_x, src_y);
        }
    }

    free(image->data);
    image->data = new_data;
    image->width = new_width;
    image->height = new_height;
    image->capacity = new_width * new_height;
}

// Функции для работы с цветом
Color color_create(float r, float g, float b) {
    Color c = {r, g, b};
    return c;
}

Color color_add(Color c1, Color c2) {
    Color result = {
        c1.r + c2.r,
        c1.g + c2.g,
        c1.b + c2.b
    };
    return result;
}

Color color_sub(Color c1, Color c2) {
    Color result = {
        c1.r - c2.r,
        c1.g - c2.g,
        c1.b - c2.b
    };
    return result;
}

Color color_mul(Color c, float scalar) {
    Color result = {
        c.r * scalar,
        c.g * scalar,
        c.b * scalar
    };
    return result;
}

Color color_clamp(Color c) {
    Color result = c;

    if (result.r < 0.0f) result.r = 0.0f;
    if (result.r > 1.0f) result.r = 1.0f;

    if (result.g < 0.0f) result.g = 0.0f;
    if (result.g > 1.0f) result.g = 1.0f;

    if (result.b < 0.0f) result.b = 0.0f;
    if (result.b > 1.0f) result.b = 1.0f;

    return result;
}

float color_luminance(Color c) {
    return 0.299f * c.r + 0.587f * c.g + 0.114f * c.b;
}