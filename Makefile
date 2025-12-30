# Компилятор для Windows
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O2 -D_CRT_SECURE_NO_WARNINGS
TARGET = image_craft.exe

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
	del /Q $(subst /,\,$(OBJS)) $(TARGET) 2>nul || true
	del /Q *.bmp 2>nul || true

# Запуск
run: $(TARGET)
	.\$(TARGET)

# Тестирование
test: $(TARGET)
	@echo Running tests...
	@if exist tests\test_scripts\test.bat (
		cd tests && test_scripts\test.bat
	) else (
		.\$(TARGET) tests\test_images\lenna.bmp test_output.bmp -gs
		echo Test completed. Check test_output.bmp
	)

.PHONY: all clean run test