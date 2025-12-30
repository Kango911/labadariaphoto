#include "filters.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

// Crop filter
void filter_crop(Image* image, void* params) {
    if (!image || !params) {
        return;
    }

    CropParams* crop = (CropParams*)params;
    int new_width = crop->width;
    int new_height = crop->height;

    // Ограничение размеров до размеров изображения
    if (new_width > image->width) new_width = image->width;
    if (new_height > image->height) new_height = image->height;

    if (new_width <= 0 || new_height <= 0) {
        return;
    }

    // Создание нового изображения с обрезанными размерами
    Image* cropped = image_create(new_width, new_height);
    if (!cropped) {
        return;
    }

    // Копирование верхней левой части
    for (int y = 0; y < new_height; y++) {
        for (int x = 0; x < new_width; x++) {
            Color color = image_get_pixel(image, x, y);
            image_set_pixel(cropped, x, y, color);
        }
    }

    // Замена данных изображения
    free(image->data);
    image->data = cropped->data;
    image->width = cropped->width;
    image->height = cropped->height;
    image->capacity = cropped->capacity;
    free(cropped);
}

// Grayscale filter
void filter_grayscale(Image* image, void* params) {
    if (!image) {
        return;
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);
            float luminance = color_luminance(color);
            Color gray = color_create(luminance, luminance, luminance);
            image_set_pixel(image, x, y, gray);
        }
    }
}

// Negative filter
void filter_negative(Image* image, void* params) {
    if (!image) {
        return;
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);
            Color negative = color_create(
                1.0f - color.r,
                1.0f - color.g,
                1.0f - color.b
            );
            image_set_pixel(image, x, y, negative);
        }
    }
}

// Sharpening filter
void filter_sharpening(Image* image, void* params) {
    float kernel[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    apply_matrix_filter(image, kernel);
}

// Edge detection filter
void filter_edge_detection(Image* image, void* params) {
    if (!image || !params) {
        return;
    }

    EdgeParams* edge = (EdgeParams*)params;
    float threshold = edge->threshold;

    // Сначала преобразуем в градации серого
    filter_grayscale(image, NULL);

    float kernel[3][3] = {
        { 0, -1,  0},
        {-1,  4, -1},
        { 0, -1,  0}
    };

    // Применяем матричный фильтр
    apply_matrix_filter(image, kernel);

    // Бинаризация по порогу
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);
            float value = color.r; // Все каналы одинаковы после grayscale

            if (value > threshold) {
                image_set_pixel(image, x, y, color_create(1.0f, 1.0f, 1.0f));
            } else {
                image_set_pixel(image, x, y, color_create(0.0f, 0.0f, 0.0f));
            }
        }
    }
}

// Median filter
void filter_median(Image* image, void* params) {
    if (!image || !params) {
        return;
    }

    MedianParams* med = (MedianParams*)params;
    int window = med->window_size;

    if (window % 2 == 0 || window < 1) {
        return;
    }

    int half = window / 2;
    Image* temp = image_copy(image);
    if (!temp) {
        return;
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            // Сбор цветов в окрестности
            int count = 0;
            Color* colors = (Color*)malloc(sizeof(Color) * window * window);

            for (int dy = -half; dy <= half; dy++) {
                for (int dx = -half; dx <= half; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;

                    // Обработка границ: используем ближайший пиксель
                    if (nx < 0) nx = 0;
                    if (nx >= image->width) nx = image->width - 1;
                    if (ny < 0) ny = 0;
                    if (ny >= image->height) ny = image->height - 1;

                    colors[count++] = image_get_pixel(temp, nx, ny);
                }
            }

            // Находим медиану для каждого канала отдельно
            float r_vals[count], g_vals[count], b_vals[count];
            for (int i = 0; i < count; i++) {
                r_vals[i] = colors[i].r;
                g_vals[i] = colors[i].g;
                b_vals[i] = colors[i].b;
            }

            // Сортировка для нахождения медианы
            for (int i = 0; i < count - 1; i++) {
                for (int j = i + 1; j < count; j++) {
                    if (r_vals[i] > r_vals[j]) {
                        float temp_val = r_vals[i];
                        r_vals[i] = r_vals[j];
                        r_vals[j] = temp_val;
                    }
                    if (g_vals[i] > g_vals[j]) {
                        float temp_val = g_vals[i];
                        g_vals[i] = g_vals[j];
                        g_vals[j] = temp_val;
                    }
                    if (b_vals[i] > b_vals[j]) {
                        float temp_val = b_vals[i];
                        b_vals[i] = b_vals[j];
                        b_vals[j] = temp_val;
                    }
                }
            }

            Color median = color_create(
                r_vals[count / 2],
                g_vals[count / 2],
                b_vals[count / 2]
            );

            image_set_pixel(image, x, y, median);
            free(colors);
        }
    }

    image_destroy(temp);
}

// Gaussian blur filter
void filter_gaussian_blur(Image* image, void* params) {
    if (!image || !params) {
        return;
    }

    BlurParams* blur = (BlurParams*)params;
    apply_gaussian_blur(image, blur->sigma);
}

// Sepia filter (дополнительный)
void filter_sepia(Image* image, void* params) {
    if (!image) {
        return;
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);

            // Формула сепии
            float new_r = color.r * 0.393f + color.g * 0.769f + color.b * 0.189f;
            float new_g = color.r * 0.349f + color.g * 0.686f + color.b * 0.168f;
            float new_b = color.r * 0.272f + color.g * 0.534f + color.b * 0.131f;

            Color sepia = color_create(new_r, new_g, new_b);
            image_set_pixel(image, x, y, sepia);
        }
    }
}

// Vignette filter (дополнительный)
void filter_vignette(Image* image, void* params) {
    if (!image) {
        return;
    }

    VignetteParams* vignette = (VignetteParams*)params;
    float intensity = vignette ? vignette->intensity : 0.8f;

    float center_x = image->width / 2.0f;
    float center_y = image->height / 2.0f;
    float max_distance = sqrtf(center_x * center_x + center_y * center_y);

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);

            float dx = x - center_x;
            float dy = y - center_y;
            float distance = sqrtf(dx * dx + dy * dy);
            float factor = 1.0f - (distance / max_distance) * intensity;

            if (factor < 0.0f) factor = 0.0f;

            Color vignetted = color_create(
                color.r * factor,
                color.g * factor,
                color.b * factor
            );

            image_set_pixel(image, x, y, vignetted);
        }
    }
}

// Вспомогательная функция для применения матричного фильтра
void apply_matrix_filter(Image* image, float kernel[3][3]) {
    if (!image) {
        return;
    }

    Image* temp = image_copy(image);
    if (!temp) {
        return;
    }

    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color sum = color_create(0, 0, 0);

            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int nx = x + kx;
                    int ny = y + ky;

                    // Обработка границ
                    if (nx < 0) nx = 0;
                    if (nx >= image->width) nx = image->width - 1;
                    if (ny < 0) ny = 0;
                    if (ny >= image->height) ny = image->height - 1;

                    Color pixel = image_get_pixel(temp, nx, ny);
                    float weight = kernel[ky + 1][kx + 1];

                    sum = color_add(sum, color_mul(pixel, weight));
                }
            }

            image_set_pixel(image, x, y, sum);
        }
    }

    image_destroy(temp);
}

// Вспомогательная функция для гауссова размытия
void apply_gaussian_blur(Image* image, float sigma) {
    if (!image || sigma <= 0) {
        return;
    }

    // Рассчитываем размер ядра (3σ в каждую сторону)
    int kernel_size = (int)(6 * sigma) | 1; // Нечетный размер
    if (kernel_size < 3) kernel_size = 3;

    int half = kernel_size / 2;
    float* kernel = (float*)malloc(sizeof(float) * kernel_size);
    float sum = 0.0f;

    // Создаем 1D ядро Гаусса
    for (int i = 0; i < kernel_size; i++) {
        int x = i - half;
        kernel[i] = expf(-(x * x) / (2 * sigma * sigma));
        sum += kernel[i];
    }

    // Нормализация
    for (int i = 0; i < kernel_size; i++) {
        kernel[i] /= sum;
    }

    // Применяем раздельно по горизонтали и вертикали
    Image* temp = image_copy(image);
    if (!temp) {
        free(kernel);
        return;
    }

    // Горизонтальное размытие
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color sum_color = color_create(0, 0, 0);

            for (int k = -half; k <= half; k++) {
                int nx = x + k;
                if (nx < 0) nx = 0;
                if (nx >= image->width) nx = image->width - 1;

                Color pixel = image_get_pixel(temp, nx, y);
                sum_color = color_add(sum_color, color_mul(pixel, kernel[k + half]));
            }

            image_set_pixel(image, x, y, sum_color);
        }
    }

    // Копируем результат для вертикального размытия
    memcpy(temp->data, image->data, sizeof(Color) * image->width * image->height);

    // Вертикальное размытие
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            Color sum_color = color_create(0,