cmake_minimum_required(VERSION 3.10)
project(image_craft C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Флаги компилятора
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS_DEBUG "-g")
set(CMAKE_C_FLAGS_RELEASE "-O3")

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

# Подключение математической библиотеки
target_link_libraries(image_craft m)