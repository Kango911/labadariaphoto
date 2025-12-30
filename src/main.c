#include <stdio.h>
#include <stdlib.h>
#include "image.h"
#include "bmp.h"
#include "cli.h"
#include "pipeline.h"

int main(int argc, char** argv) {
    printf("=== ImageCraft - Laboratory Work #1 ===\n");

    // Парсинг аргументов командной строки
    CLIArgs* args = cli_parse_args(argc, argv);
    if (!args) {
        return EXIT_FAILURE;
    }

    if (args->show_help) {
        cli_free_args(args);
        return EXIT_SUCCESS;
    }

    // Проверка формата входного файла
    if (!bmp_is_valid_format(args->input_file)) {
        fprintf(stderr, "Error: %s is not a valid BMP file\n", args->input_file);
        cli_free_args(args);
        return EXIT_FAILURE;
    }

    // Чтение изображения
    printf("Reading image from %s...\n", args->input_file);
    Image* image = bmp_read(args->input_file);
    if (!image) {
        fprintf(stderr, "Error: Cannot read image from %s\n", args->input_file);
        cli_free_args(args);
        return EXIT_FAILURE;
    }

    printf("Image loaded: %d x %d pixels\n", image->width, image->height);

    // Применение фильтров
    if (args->pipeline->count > 0) {
        printf("\nApplying %d filter(s)...\n", args->pipeline->count);
        pipeline_apply(args->pipeline, image);
    } else {
        printf("\nNo filters specified, saving original image...\n");
    }

    // Сохранение изображения
    printf("\nSaving image to %s...\n", args->output_file);
    if (!bmp_write(args->output_file, image)) {
        fprintf(stderr, "Error: Cannot save image to %s\n", args->output_file);
        image_destroy(image);
        cli_free_args(args);
        return EXIT_FAILURE;
    }

    // Очистка
    image_destroy(image);
    cli_free_args(args);

    printf("Done!\n");
    return EXIT_SUCCESS;
}