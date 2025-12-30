#include "bmp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Image* bmp_read(const char* filename) {
    if (!filename) {
        return NULL;
    }

    FILE* file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    // Чтение заголовков
    BMPFileHeader file_header;
    BMPInfoHeader info_header;

    if (fread(&file_header, sizeof(BMPFileHeader), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Error: Cannot read BMP file header\n");
        return NULL;
    }

    // Проверка сигнатуры
    if (file_header.signature != 0x4D42) { // 'BM'
        fclose(file);
        fprintf(stderr, "Error: Invalid BMP signature\n");
        return NULL;
    }

    if (fread(&info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Error: Cannot read BMP info header\n");
        return NULL;
    }

    // Проверка формата (только 24-битные без сжатия)
    if (info_header.bits_per_pixel != 24 || info_header.compression != 0) {
        fclose(file);
        fprintf(stderr, "Error: Only 24-bit uncompressed BMP supported\n");
        return NULL;
    }

    // Переход к данным изображения
    fseek(file, file_header.data_offset, SEEK_SET);

    // Создание изображения
    Image* image = image_create(info_header.width, abs(info_header.height));
    if (!image) {
        fclose(file);
        fprintf(stderr, "Error: Cannot create image\n");
        return NULL;
    }

    // Расчет выравнивания строк
    int row_padding = (4 - (info_header.width * 3) % 4) % 4;

    // Чтение данных пикселей
    for (int y = image->height - 1; y >= 0; y--) {
        for (int x = 0; x < image->width; x++) {
            uint8_t pixel[3];
            if (fread(pixel, 3, 1, file) != 1) {
                image_destroy(image);
                fclose(file);
                fprintf(stderr, "Error: Cannot read pixel data\n");
                return NULL;
            }

            // BMP хранит цвета в порядке BGR
            Color color = color_create(
                pixel[2] / 255.0f, // R
                pixel[1] / 255.0f, // G
                pixel[0] / 255.0f  // B
            );

            image_set_pixel(image, x, y, color);
        }

        // Пропуск выравнивания
        fseek(file, row_padding, SEEK_CUR);
    }

    fclose(file);
    return image;
}

bool bmp_write(const char* filename, const Image* image) {
    if (!filename || !image) {
        return false;
    }

    FILE* file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot create file %s\n", filename);
        return false;
    }

    // Расчет выравнивания строк
    int row_padding = (4 - (image->width * 3) % 4) % 4;
    int row_size = image->width * 3 + row_padding;
    int image_size = row_size * image->height;

    // Заголовок файла
    BMPFileHeader file_header = {
        .signature = 0x4D42, // 'BM'
        .file_size = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + image_size,
        .reserved = 0,
        .data_offset = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader)
    };

    // Информационный заголовок
    BMPInfoHeader info_header = {
        .header_size = sizeof(BMPInfoHeader),
        .width = image->width,
        .height = image->height,
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
    if (fwrite(&file_header, sizeof(BMPFileHeader), 1, file) != 1 ||
        fwrite(&info_header, sizeof(BMPInfoHeader), 1, file) != 1) {
        fclose(file);
        fprintf(stderr, "Error: Cannot write BMP headers\n");
        return false;
    }

    // Запись данных пикселей
    uint8_t padding[3] = {0, 0, 0};

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
                fclose(file);
                fprintf(stderr, "Error: Cannot write pixel data\n");
                return false;
            }
        }

        // Запись выравнивания
        if (row_padding > 0 && fwrite(padding, row_padding, 1, file) != 1) {
            fclose(file);
            fprintf(stderr, "Error: Cannot write padding\n");
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