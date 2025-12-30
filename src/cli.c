#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

CLIArgs* cli_parse_args(int argc, char** argv) {
    if (argc < 2) {
        cli_print_help();
        return NULL;
    }

    CLIArgs* args = (CLIArgs*)calloc(1, sizeof(CLIArgs));
    if (!args) {
        return NULL;
    }

    args->pipeline = pipeline_create();
    if (!args->pipeline) {
        free(args);
        return NULL;
    }

    // Парсинг аргументов
    int i = 1;
    while (i < argc) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            args->show_help = 1;
            cli_print_help();
            return args;
        }

        // Первый аргумент - входной файл
        if (!args->input_file) {
            args->input_file = strdup(argv[i]);
        }
        // Второй аргумент - выходной файл
        else if (!args->output_file) {
            args->output_file = strdup(argv[i]);
        }
        // Фильтры
        else if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-crop") == 0) {
                if (i + 2 >= argc) {
                    fprintf(stderr, "Error: -crop requires width and height\n");
                    cli_free_args(args);
                    return NULL;
                }

                CropParams* params = (CropParams*)malloc(sizeof(CropParams));
                params->width = atoi(argv[++i]);
                params->height = atoi(argv[++i]);

                pipeline_add_filter(args->pipeline, filter_crop, params, "crop");
            }
            else if (strcmp(argv[i], "-gs") == 0) {
                pipeline_add_filter(args->pipeline, filter_grayscale, NULL, "grayscale");
            }
            else if (strcmp(argv[i], "-neg") == 0) {
                pipeline_add_filter(args->pipeline, filter_negative, NULL, "negative");
            }
            else if (strcmp(argv[i], "-sharp") == 0) {
                pipeline_add_filter(args->pipeline, filter_sharpening, NULL, "sharpening");
            }
            else if (strcmp(argv[i], "-edge") == 0) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "Error: -edge requires threshold\n");
                    cli_free_args(args);
                    return NULL;
                }

                EdgeParams* params = (EdgeParams*)malloc(sizeof(EdgeParams));
                params->threshold = atof(argv[++i]);

                pipeline_add_filter(args->pipeline, filter_edge_detection, params, "edge_detection");
            }
            else if (strcmp(argv[i], "-med") == 0) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "Error: -med requires window size\n");
                    cli_free_args(args);
                    return NULL;
                }

                MedianParams* params = (MedianParams*)malloc(sizeof(MedianParams));
                params->window_size = atoi(argv[++i]);

                pipeline_add_filter(args->pipeline, filter_median, params, "median");
            }
            else if (strcmp(argv[i], "-blur") == 0) {
                if (i + 1 >= argc) {
                    fprintf(stderr, "Error: -blur requires sigma\n");
                    cli_free_args(args);
                    return NULL;
                }

                BlurParams* params = (BlurParams*)malloc(sizeof(BlurParams));
                params->sigma = atof(argv[++i]);

                pipeline_add_filter(args->pipeline, filter_gaussian_blur, params, "gaussian_blur");
            }
            else if (strcmp(argv[i], "-sepia") == 0) {
                pipeline_add_filter(args->pipeline, filter_sepia, NULL, "sepia");
            }
            else if (strcmp(argv[i], "-vignette") == 0) {
                VignetteParams* params = (VignetteParams*)malloc(sizeof(VignetteParams));
                params->intensity = 0.8f; // Значение по умолчанию

                // Проверяем, есть ли параметр интенсивности
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    params->intensity = atof(argv[++i]);
                }

                pipeline_add_filter(args->pipeline, filter_vignette, params, "vignette");
            }
            else {
                fprintf(stderr, "Error: Unknown filter %s\n", argv[i]);
                cli_free_args(args);
                return NULL;
            }
        }
        else {
            fprintf(stderr, "Error: Unexpected argument %s\n", argv[i]);
            cli_free_args(args);
            return NULL;
        }

        i++;
    }

    // Проверка обязательных аргументов
    if (!args->input_file || !args->output_file) {
        fprintf(stderr, "Error: Input and output files are required\n");
        cli_print_help();
        cli_free_args(args);
        return NULL;
    }

    return args;
}

void cli_free_args(CLIArgs* args) {
    if (!args) {
        return;
    }

    if (args->input_file) free(args->input_file);
    if (args->output_file) free(args->output_file);
    if (args->pipeline) pipeline_destroy(args->pipeline);
    free(args);
}

void cli_print_help(void) {
    printf("ImageCraft - Image Processing Tool\n");
    printf("Usage: image_craft <input.bmp> <output.bmp> [filters...]\n\n");
    printf("Filters:\n");
    printf("  -crop <width> <height>    Crop image\n");
    printf("  -gs                       Convert to grayscale\n");
    printf("  -neg                      Convert to negative\n");
    printf("  -sharp                    Apply sharpening\n");
    printf("  -edge <threshold>         Edge detection\n");
    printf("  -med <window_size>        Median filter\n");
    printf("  -blur <sigma>             Gaussian blur\n");
    printf("  -sepia                    Apply sepia tone\n");
    printf("  -vignette [intensity]     Apply vignette effect (default: 0.8)\n");
    printf("\nExamples:\n");
    printf("  image_craft input.bmp output.bmp -crop 800 600 -gs\n");
    printf("  image_craft input.bmp output.bmp -blur 0.5 -sharp\n");
    printf("  image_craft input.bmp output.bmp -edge 0.1\n");
}