#ifndef PIPELINE_H
#define PIPELINE_H

#include "image.h"
#include "filters.h"

// Тип функции фильтра
typedef void (*FilterFunc)(Image*, void*);

// Структура для представления фильтра в пайплайне
typedef struct FilterNode {
    FilterFunc function;
    void* params;
    char* name;
    struct FilterNode* next;
} FilterNode;

// Структура пайплайна
typedef struct {
    FilterNode* head;
    FilterNode* tail;
    int count;
} FilterPipeline;

// Создание и уничтожение пайплайна
FilterPipeline* pipeline_create(void);
void pipeline_destroy(FilterPipeline* pipeline);

// Добавление фильтра в пайплайн
void pipeline_add_filter(FilterPipeline* pipeline,
                        FilterFunc function,
                        void* params,
                        const char* name);

// Применение пайплайна к изображению
void pipeline_apply(FilterPipeline* pipeline, Image* image);

// Очистка пайплайна
void pipeline_clear(FilterPipeline* pipeline);

// Получение количества фильтров
int pipeline_get_count(const FilterPipeline* pipeline);

#endif // PIPELINE_H