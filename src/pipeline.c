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
        fprintf(stderr, "Error: Cannot add filter to pipeline (NULL parameters)\n");
        return;
    }

    FilterNode* node = (FilterNode*)malloc(sizeof(FilterNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed for filter node\n");
        return;
    }

    node->function = function;
    node->params = params;
    node->next = NULL;

    // Копируем имя фильтра
    if (name) {
        node->name = _strdup(name);
    } else {
        node->name = _strdup("unnamed");
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
    printf("Added filter: %s\n", node->name);
}

void pipeline_apply(FilterPipeline* pipeline, Image* image) {
    if (!pipeline || !image) {
        fprintf(stderr, "Error: Cannot apply pipeline (NULL parameters)\n");
        return;
    }

    if (pipeline->count == 0) {
        printf("No filters to apply\n");
        return;
    }

    printf("\nApplying %d filter(s):\n", pipeline->count);
    printf("========================================\n");

    FilterNode* current = pipeline->head;
    int filter_index = 1;

    while (current) {
        printf("Filter %d/%d: %s\n", filter_index++, pipeline->count, current->name);

        // Применяем фильтр
        if (current->function) {
            current->function(image, current->params);
        } else {
            fprintf(stderr, "Warning: Filter function is NULL for %s\n", current->name);
        }

        current = current->next;
    }

    printf("========================================\n");
    printf("All filters applied successfully\n\n");
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

int pipeline_get_count(const FilterPipeline* pipeline) {
    return pipeline ? pipeline->count : 0;
}