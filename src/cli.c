#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

CLIArgs* cli_parse_args(int argc, char** argv) {
    CLIArgs* args = (CLIArgs*)calloc(1, sizeof(CLIArgs));
    if (!args) {
        return NULL;
    }

    args->pipeline = pipeline_create();
    if (!args->pipeline) {
        free(args);
        return NULL;
    }

    // Если нет аргументов - показываем помощь
    if (argc < 2) {
        args->show_help = 1;
        return args;
    }

    // Парсинг аргументов
    int i = 1;
    while (i < argc) {
        // Помощь
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 ||
            strcmp(argv[i], "/?") == 0) {
            args->show_help = 1;
            return args;
        }

        // Первый аргумент - входной файл
        if (!args->input_file) {
            args->input_file = _strdup(argv[i]);
        }
        // Второй аргумент - выходной файл
        else if (!args->output_file) {
            args->output_file = _strdup(argv[i]);
        }
        // Фильтры
        else if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-crop") == 0) {
                if (i + 2 >= argc) {
                    args->error = 1;
                    args->error_message = "-crop requires width and height";
                    return args;
                }

                CropParams* params = (CropParams*)malloc(sizeof(CropParams));
                if (!params) {
                    args->error = 1;
                    args->error_message = "Memory allocation failed";
                    return args;
                }

                params->width = atoi(argv[i + 1]);
                params->height = atoi(argv[i + 2]);

                if (params->width <= 0 || params->height <= 0) {
                    free(params);
                    args->error = 1;
                    args->error_message = "Crop dimensions must be positive";
                    return args;
                }

                pipeline_add_filter(args->pipeline, filter_crop, params, "crop");
                i += 2;
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
                    args->error = 1;
                    args->error_message = "-edge requires threshold";
                    return args;
                }

                EdgeParams* params = (EdgeParams*)malloc(sizeof(EdgeParams));
                if (!params) {
                    args->error = 1;
                    args->error_message = "Memory allocation failed";
                    return args;
                }

                params->threshold = (float)atof(argv[i + 1]);

                if (params->threshold < 0 || params->threshold > 1) {
                    free(params);
                    args->error = 1;
                    args->error_message = "Edge threshold must be between 0 and 1";
                    return args;
                }

                pipeline_add_filter(args->pipeline, filter_edge_detection, params, "edge_detection");
                i += 1;
            }
            else if (strcmp(argv[i], "-med") == 0) {
                if (i + 1 >= argc) {
                    args->error = 1;
                    args->error_message = "-med requires window size";
                    return args;
                }

                MedianParams* params = (MedianParams*)malloc(sizeof(MedianParams));
                if (!params) {
                    args->error = 1;
                    args->error_message = "Memory allocation failed";
                    return args;
                }

                params->window_size = atoi(argv[i + 1]);

                if (params->window_size <= 0 || params->window_size % 2 == 0) {
                    free(params);
                    args->error = 1;
                    args->error_message = "Median window size must be odd and positive";
                    return args;
                }

                pipeline_add_filter(args->pipeline, filter_median, params, "median");
                i += 1;
            }
            else if (strcmp(argv[i], "-blur") == 0) {
                if (i + 1 >= argc) {
                    args->error = 1;
                    args->error_message = "-blur requires sigma";
                    return args;
                }

                BlurParams* params = (BlurParams*)malloc(sizeof(BlurParams));
                if (!params) {
                    args->error = 1;
                    args->error_message = "Memory allocation failed";
                    return args;
                }

                params->sigma = (float)atof(argv[i + 1]);

                if (params->sigma <= 0) {
                    free(params);
                    args->error = 1;
                    args->error_message = "Blur sigma must be positive";
                    return args;
                }

                pipeline_add_filter(args->pipeline, filter_gaussian_blur, params, "gaussian_blur");
                i += 1;
            }
            else if (strcmp(argv[i], "-sepia") == 0) {
                pipeline_add_filter(args->pipeline, filter_sepia, NULL, "sepia");
            }
            else if (strcmp(argv[i], "-vignette") == 0) {
                VignetteParams* params = (VignetteParams*)malloc(sizeof(VignetteParams));
                if (!params) {
                    args->error = 1;
                    args->error_message = "Memory allocation failed";
                    return args;
                }

                // Значение по умолчанию
                params->intensity = 0.8f;

                // Проверяем, есть ли параметр интенсивности
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    params->intensity = (float)atof(argv[i + 1]);
                    i += 1;
                }

                pipeline_add_filter(args->pipeline, filter_vignette, params, "vignette");
            }
            else {
                args->error = 1;
                args->error_message = malloc(100);
                if (args->error_message) {
                    sprintf(args->error_message, "Unknown filter: %s", argv[i]);
                }
                return args;
            }
        }
        else {
            args->error = 1;
            args->error_message = malloc(100);
            if (args->error_message) {
                sprintf(args->error_message, "Unexpected argument: %s", argv[i]);
            }
            return args;
        }

        i++;
    }

    // Проверка обязательных аргументов
    if (!args->input_file || !args->output_file) {
        args->error = 1;
        args->error_message = "Input and output files are required";
        return args;
    }

    // Проверка расширений файлов
    if (strstr(args->input_file, ".bmp") == NULL &&
        strstr(args->input_file, ".BMP") == NULL) {
        args->error = 1;
        args->error_message = "Input file must have .bmp extension";
        return args;
    }

    if (strstr(args->output_file, ".bmp") == NULL &&
        strstr(args->output_file, ".BMP") == NULL) {
        args->error = 1;
        args->error_message = "Output file must have .bmp extension";
        return args;
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
    if (args->error_message) free(args->error_message);
    free(args);
}

void cli_print_help(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                 ImageCraft - Лабораторная работа №1     ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Использование:\n");
    printf("  image_craft.exe <input.bmp> <output.bmp> [фильтры...]\n");
    printf("\n");
    printf("Фильтры:\n");
    printf("  -crop <ширина> <высота>   Обрезать изображение\n");
    printf("  -gs                       Градации серого\n");
    printf("  -neg                      Негатив\n");
    printf("  -sharp                    Повышение резкости\n");
    printf("  -edge <порог>             Обнаружение границ (0-1)\n");
    printf("  -med <размер_окна>        Медианный фильтр (нечетный)\n");
    printf("  -blur <сигма>             Гауссово размытие\n");
    printf("  -sepia                    Эффект сепии\n");
    printf("  -vignette [интенсивность] Виньетирование (0-1, по умолчанию 0.8)\n");
    printf("\n");
    printf("Примеры:\n");
    printf("  image_craft.exe input.bmp output.bmp -gs\n");
    printf("  image_craft.exe input.bmp output.bmp -crop 800 600 -gs -blur 0.5\n");
    printf("  image_craft.exe input.bmp output.bmp -edge 0.1 -neg\n");
    printf("  image_craft.exe input.bmp output.bmp -sepia -vignette 0.7\n");
    printf("\n");
    printf("Формат изображений: 24-битный BMP без сжатия\n");
    printf("\n");
}

void cli_print_error(const CLIArgs* args) {
    if (!args || !args->error_message) {
        return;
    }

    printf("\n❌ ОШИБКА: %s\n\n", args->error_message);
    printf("Используйте -h для справки\n\n");
}