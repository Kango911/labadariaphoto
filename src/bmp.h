#ifndef BMP_H
#define BMP_H

#include "image.h"
#include <stdbool.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t signature;      // 'BM'
    uint32_t file_size;
    uint32_t reserved;
    uint32_t data_offset;
} BMPFileHeader;

typedef struct {
    uint32_t header_size;    // 40
    int32_t width;
    int32_t height;
    uint16_t planes;         // 1
    uint16_t bits_per_pixel; // 24
    uint32_t compression;    // 0
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t important_colors;
} BMPInfoHeader;
#pragma pack(pop)

// Чтение BMP файла
Image* bmp_read(const char* filename);

// Запись BMP файла
bool bmp_write(const char* filename, const Image* image);

// Проверка формата файла
bool bmp_is_valid_format(const char* filename);

#endif // BMP_H