#ifndef FILTERS_H
#define FILTERS_H

#include "image.h"

// Структуры параметров для фильтров
typedef struct {
    int width;
    int height;
} CropParams;

typedef struct {
    float threshold;
} EdgeParams;

typedef struct {
    int window_size;
} MedianParams;

typedef struct {
    float sigma;
} BlurParams;

typedef struct {
    float intensity;
} VignetteParams;

// Базовые фильтры
void filter_crop(Image* image, void* params);
void filter_grayscale(Image* image, void* params);
void filter_negative(Image* image, void* params);
void filter_sharpening(Image* image, void* params);
void filter_edge_detection(Image* image, void* params);
void filter_median(Image* image, void* params);
void filter_gaussian_blur(Image* image, void* params);

// Дополнительные фильтры
void filter_sepia(Image* image, void* params);
void filter_vignette(Image* image, void* params);

// Вспомогательные функции
void apply_matrix_filter(Image* image, float kernel[3][3], float divisor);
void apply_gaussian_blur(Image* image, float sigma);
Color get_median_color(Color* colors, int count);

// Утилиты фильтров
void filter_box_blur(Image* image, int radius);
void filter_emboss(Image* image);

#endif // FILTERS_H