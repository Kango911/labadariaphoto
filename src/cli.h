#ifndef CLI_H
#define CLI_H

#include "pipeline.h"

// Структура для аргументов командной строки
typedef struct {
    char* input_file;
    char* output_file;
    FilterPipeline* pipeline;
    int show_help;
    int error;
    char* error_message;
} CLIArgs;

// Парсинг аргументов командной строки
CLIArgs* cli_parse_args(int argc, char** argv);

// Освобождение памяти аргументов
void cli_free_args(CLIArgs* args);

// Вывод справки
void cli_print_help(void);

// Вывод ошибки
void cli_print_error(const CLIArgs* args);

#endif // CLI_H