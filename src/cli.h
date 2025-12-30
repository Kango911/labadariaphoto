#ifndef CLI_H
#define CLI_H

#include "pipeline.h"

// Структура для аргументов командной строки
typedef struct {
    char* input_file;
    char* output_file;
    FilterPipeline* pipeline;
    int show_help;
} CLIArgs;

// Парсинг аргументов командной строки
CLIArgs* cli_parse_args(int argc, char** argv);

// Освобождение памяти аргументов
void cli_free_args(CLIArgs* args);

// Вывод справки
void cli_print_help(void);

#endif // CLI_H