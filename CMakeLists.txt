cmake_minimum_required(VERSION 3.10)
project(image_craft C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Настройки для Windows
if(WIN32)
    add_definitions(-D_WIN32 -D_CRT_SECURE_NO_WARNINGS)
endif()

# Исходные файлы
set(SOURCES
        src/main.c
        src/image.c
        src/bmp.c
        src/filters.c
        src/pipeline.c
        src/cli.c
)

# Заголовочные файлы
set(HEADERS
        src/image.h
        src/bmp.h
        src/filters.h
        src/pipeline.h
        src/cli.h
)

# Создание исполняемого файла
add_executable(image_craft ${SOURCES} ${HEADERS})

# Настройки компилятора
target_compile_options(image_craft PRIVATE -Wall -Wextra -Werror -O2)

# Для Windows нужна математическая библиотека
if(WIN32)
    target_link_libraries(image_craft m)
else()
    target_link_libraries(image_craft m)
endif()

# Копирование тестовых изображений
file(COPY tests/test_images DESTINATION ${CMAKE_BINARY_DIR}/tests)