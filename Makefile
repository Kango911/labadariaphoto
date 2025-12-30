# Компилятор и флаги
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -lm -g
TARGET = image_craft

# Исходные файлы
SRC_DIR = src
SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/image.c \
       $(SRC_DIR)/bmp.c \
       $(SRC_DIR)/filters.c \
       $(SRC_DIR)/pipeline.c \
       $(SRC_DIR)/cli.c

OBJS = $(SRCS:.c=.o)

# Правила сборки
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Компиляция каждого .c файла
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)
	rm -f test_*.bmp

# Тестирование
test: $(TARGET)
	@echo "Running tests..."
	@if [ -f tests/test_scripts/test.sh ]; then \
		cd tests && ./test_scripts/test.sh; \
	else \
		./image_craft tests/test_images/lenna.bmp test_output.bmp -gs; \
		echo "Test completed. Check test_output.bmp"; \
	fi

# Форматирование кода
format:
	find src -name "*.c" -o -name "*.h" | xargs clang-format -i

# Сборка релизной версии
release: CFLAGS = -std=c11 -Wall -Wextra -O3 -lm -DNDEBUG
release: clean $(TARGET)

.PHONY: all clean test format release