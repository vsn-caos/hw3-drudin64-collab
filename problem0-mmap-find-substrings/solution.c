#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Программе передаются два аргумента: имя файла и строка для поиска.
// Необходимо найти все вхождения строки в текстовом файле,
// используя отображение на память с помощью системного вызова mmap.
// На стандартный поток вывода вывести список всех позиций (с нуля),
// где встречается искомая строка, по одной на строку.

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <filename> <search_string>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);

    size_t file_size = st.st_size;
    size_t str_size = strlen(argv[2]);
    char *data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);

    for (size_t i = 0; i + str_size <= file_size; i++) {
        if (memcmp(data + i, argv[2], str_size) == 0) {
            printf("%zu\n", i);
        }
    }

    munmap(data, file_size);
    close(fd);

    return 0;
}
