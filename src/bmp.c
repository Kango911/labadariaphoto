#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

Image* bmp_read(const char* filename) {
    if (!filename) {
        fprintf(stderr, "Error: Filename is NULL\n");
        return NULL;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s': %s\n", filename, strerror(errno));
        return NULL;
    }

    // Чтение заголовков
    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    if (fread(&file_header, sizeof(BMPFileHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot read BMP file header from '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Проверка сигнатуры
    if (file_header.signature != 0x4D42) { // 'BM'
        fprintf(stderr, "Error: Invalid BMP signature in '%s' (expected 'BM')\n", filename);
        fclose(file);
        return NULL;
    }

    if (fread(&info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot read BMP info header from '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Проверка формата (только 24-битные без сжатия)
    if (info_header.bits_per_pixel != 24) {
        fprintf(stderr, "Error: Only 24-bit BMP supported (got %d-bit) in '%s'\n",
                info_header.bits_per_pixel, filename);
        fclose(file);
        return NULL;
    }

    if (info_header.compression != 0) {
        fprintf(stderr, "Error: Only uncompressed BMP supported in '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Переход к данным изображения
    if (fseek(file, file_header.data_offset, SEEK_SET) != 0) {
        fprintf(stderr, "Error: Cannot seek to pixel data in '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Создание изображения
    int width = info_header.width;
    int height = abs(info_header.height); // Обрабатываем отрицательную высоту

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Error: Invalid image dimensions %dx%d in '%s'\n",
                width, height, filename);
        fclose(file);
        return NULL;
    }

    Image* image = image_create(width, height);
    if (!image) {
        fprintf(stderr, "Error: Cannot create image structure for '%s'\n", filename);
        fclose(file);
        return NULL;
    }

    // Расчет выравнивания строк
    int row_padding = (4 - (width * 3) % 4) % 4;

    // Определяем порядок строк (снизу вверх или сверху вниз)
    int is_top_down = info_header.height < 0;

    // Чтение данных пикселей
    for (int y = 0; y < height; y++) {
        int target_y = is_top_down ? y : (height - 1 - y);

        for (int x = 0; x < width; x++) {
            uint8_t pixel[3];
            if (fread(pixel, 3, 1, file) != 1) {
                fprintf(stderr, "Error: Cannot read pixel data at (%d, %d) in '%s'\n",
                        x, y, filename);
                image_destroy(image);
                fclose(file);
                return NULL;
            }

            // BMP хранит цвета в порядке BGR
            Color color = color_create(
                pixel[2] / 255.0f, // R
                pixel[1] / 255.0f, // G
                pixel[0] / 255.0f  // B
            );

            image_set_pixel(image, x, target_y, color);
        }

        // Пропуск выравнивания
        if (row_padding > 0) {
            if (fseek(file, row_padding, SEEK_CUR) != 0) {
                fprintf(stderr, "Error: Cannot skip padding in '%s'\n", filename);
                image_destroy(image);
                fclose(file);
                return NULL;
            }
        }
    }

    fclose(file);
    return image;
}

bool bmp_write(const char* filename, const Image* image) {
    if (!filename || !image) {
        fprintf(stderr, "Error: Invalid parameters for bmp_write\n");
        return false;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot create file '%s': %s\n", filename, strerror(errno));
        return false;
    }

    // Расчет выравнивания строк
    int row_padding = (4 - (image->width * 3) % 4) % 4;
    int row_size = image->width * 3 + row_padding;
    int image_size = row_size * image->height;
    int file_size = 54 + image_size;

    // Заголовок файла
    BMPFileHeader file_header = {
        .signature = 0x4D42, // 'BM'
        .file_size = file_size,
        .reserved = 0,
        .data_offset = 54
    };

    // Информационный заголовок
    BMPInfoHeader info_header = {
        .header_size = 40,
        .width = image->width,
        .height = image->height, // Положительное - снизу вверх
        .planes = 1,
        .bits_per_pixel = 24,
        .compression = 0,
        .image_size = image_size,
        .x_pixels_per_meter = 2835, // 72 DPI
        .y_pixels_per_meter = 2835,
        .colors_used = 0,
        .important_colors = 0
    };

    // Запись заголовков
    if (fwrite(&file_header, sizeof(BMPFileHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot write BMP file header to '%s'\n", filename);
        fclose(file);
        return false;
    }

    if (fwrite(&info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        fprintf(stderr, "Error: Cannot write BMP info header to '%s'\n", filename);
        fclose(file);
        return false;
    }

    // Запись данных пикселей
    uint8_t padding[4] = {0, 0, 0, 0};

    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            Color color = image_get_pixel(image, x, y);

            // Преобразование в BGR и 0-255
            uint8_t pixel[3] = {
                (uint8_t)(color.b * 255),
                (uint8_t)(color.g * 255),
                (uint8_t)(color.r * 255)
            };

            if (fwrite(pixel, 3, 1, file) != 1) {
                fprintf(stderr, "Error: Cannot write pixel data to '%s'\n", filename);
                fclose(file);
                return false;
            }
        }

        // Запись выравнивания
        if (row_padding > 0 && fwrite(padding, row_padding, 1, file) != 1) {
            fprintf(stderr, "Error: Cannot write padding to '%s'\n", filename);
            fclose(file);
            return false;
        }
    }

    fclose(file);
    return true;
}

bool bmp_is_valid_format(const char* filename) {
    if (!filename) {
        return false;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        return false;
    }

    uint16_t signature;
    bool result = false;

    if (fread(&signature, sizeof(uint16_t), 1, file) == 1) {
        result = (signature == 0x4D42); // 'BM'
    }

    fclose(file);
    return result;
}

bool bmp_get_info(const char* filename, int* width, int* height) {
    if (!filename || !width || !height) {
        return false;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        return false;
    }

    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    bool success = false;

    if (fread(&file_header, sizeof(BMPFileHeader), 1, file) == 1 &&
        file_header.signature == 0x4D42 &&
        fread(&info_header, sizeof(BMPInfoHeader), 1, file) == 1) {

        *width = info_header.width;
        *height = abs(info_header.height);
        success = true;
    }

    fclose(file);
    return success;
}