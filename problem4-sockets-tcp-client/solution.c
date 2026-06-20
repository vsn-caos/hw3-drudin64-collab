#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Программе передаются два аргумента:
//   argv[1] — IPv4-адрес сервера в десятичной записи (например, "127.0.0.1")
//   argv[2] — номер порта
//
// Программа должна:
//   1. Установить TCP-соединение с указанным сервером.
//   2. В цикле читать со stdin целые знаковые числа в текстовом формате.
//   3. Отправлять каждое число на сервер в бинарном виде (int32, Little Endian).
//   4. Получать от сервера int32 LE в ответ и выводить его в stdout в текстовом виде.
//   5. Если сервер закрыл соединение — завершиться с кодом возврата 0.

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <ipv4_addr> <port>\n", argv[0]);
        return 1;
    }
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    int32_t number;
    int32_t answer;

    while (scanf("%d", &number) == 1) {
        size_t sent = 0;

        while (sent < sizeof(number)) {
            ssize_t n = send(
                sock,
                (char *)&number + sent,
                sizeof(number) - sent,
                MSG_NOSIGNAL
            );

            if (n < 0) {
                close(sock);
                return 0;
            }

            sent += n;
        }

        size_t received = 0;
        while (received < sizeof(answer)) {
            ssize_t n = recv(
                sock,
                (char *)&answer + received,
                sizeof(answer) - received,
                0
            );

            if (n <= 0) {
                close(sock);
                return 0;
            }

            received += n;
        }
        printf("%d\n", (int)answer);
    }

    close(sock);

    return 0;
}

