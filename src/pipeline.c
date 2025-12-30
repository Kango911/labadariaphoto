#include "pipeline.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

FilterPipeline* pipeline_create(void) {
    FilterPipeline* pipeline = (FilterPipeline*)malloc(sizeof(FilterPipeline));
    if (pipeline) {
        pipeline->head = NULL;
        pipeline->tail = NULL;
        pipeline->count = 0;
    }
    return pipeline;
}

void pipeline_destroy(FilterPipeline* pipeline) {
    if (!pipeline) {
        return;
    }

    pipeline_clear(pipeline);
    free(pipeline);
}

void pipeline_add_filter(FilterPipeline* pipeline,
                        FilterFunc function,
                        void* params,
                        const char* name) {
    if (!pipeline || !function) {
        return;
    }

    FilterNode* node = (FilterNode*)malloc(sizeof(FilterNode));
    if (!node) {
        return;
    }

    node->function = function;
    node->params = params;
    node->next = NULL;

    // Копируем имя фильтра
    if (name) {
        node->name = strdup(name);
    } else {
        node->name = strdup("unnamed");
    }

    // Добавляем в конец списка
    if (!pipeline->head) {
        pipeline->head = node;
        pipeline->tail = node;
    } else {
        pipeline->tail->next = node;
        pipeline->tail = node;
    }

    pipeline->count++;
}

void pipeline_apply(FilterPipeline* pipeline, Image* image) {
    if (!pipeline || !image) {
        return;
    }

    FilterNode* current = pipeline->head;
    int filter_index = 1;

    while (current) {
        printf("Applying filter %d: %s\n", filter_index++, current->name);

        // Применяем фильтр
        current->function(image, current->params);

        current = current->next;
    }
}

void pipeline_clear(FilterPipeline* pipeline) {
    if (!pipeline) {
        return;
    }

    FilterNode* current = pipeline->head;

    while (current) {
        FilterNode* next = current->next;

        // Освобождаем параметры (если они были выделены динамически)
        if (current->params) {
            free(current->params);
        }

        // Освобождаем имя
        if (current->name) {
            free(current->name);
        }

        free(current);
        current = next;
    }

    pipeline->head = NULL;
    pipeline->tail = NULL;
    pipeline->count = 0;
}