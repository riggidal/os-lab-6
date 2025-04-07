#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "log.h"

#define BUF_SIZE 10

int used = 0;
int buffer[BUF_SIZE] = {0};

HANDLE g_mutex;  // глобальный мьютекс

// Поток контроля буфера
DWORD WINAPI buffer_control(LPVOID lpParam) {
    while (1) {
        char buf_repr[256] = {0};
        int offset = 0;

        WaitForSingleObject(g_mutex, INFINITE);

        offset += sprintf(buf_repr + offset, "[");
        for (int i = 0; i < BUF_SIZE; i++) {
            offset += sprintf(buf_repr + offset, "%d", buffer[i]);
            if (i != BUF_SIZE - 1) offset += sprintf(buf_repr + offset, ", ");
        }
        offset += sprintf(buf_repr + offset, "]");
        float usage = (used / (float)BUF_SIZE) * 100;

        ReleaseMutex(g_mutex);

        log_debug("Буфер: %s", buf_repr);
        log_info("Буфер заполнен на %.0f%%", usage);

        Sleep(2000);
    }
    return 0;
}

// Поток-потребитель
DWORD WINAPI get_number(LPVOID lpParam) {
    while (1) {
        WaitForSingleObject(g_mutex, INFINITE);
        if (used > 0) {
            for (int i = 0; i < BUF_SIZE; i++) {
                if (buffer[i] != 0) {
                    log_info("[ПОТРЕБИТЕЛЬ] Считано число %d из ячейки %d", buffer[i], i);
                    buffer[i] = 0;
                    used--;
                    break;
                }
            }
        }
        ReleaseMutex(g_mutex);
        Sleep(810);
    }
    return 0;
}

// Поток-производитель
DWORD WINAPI set_number(LPVOID lpParam) {
    while (1) {
        WaitForSingleObject(g_mutex, INFINITE);
        if (used < BUF_SIZE) {
            int number = rand() % 100 + 1;
            for (int i = BUF_SIZE - 1; i >= 0; i--) {
                if (buffer[i] == 0) {
                    buffer[i] = number;
                    used++;
                    log_info("[ПРОИЗВОДИТЕЛЬ] Записано число %d в ячейку %d", number, i);
                    break;
                }
            }
        }
        ReleaseMutex(g_mutex);
        Sleep(800);
    }
    return 0;
}

int main() {
    srand((unsigned int)time(NULL));

    // Создание мьютекса
    g_mutex = CreateMutex(NULL, FALSE, NULL);
    if (g_mutex == NULL) {
        fprintf(stderr, "Не удалось создать мьютекс\n");
        return 1;
    }

    HANDLE threads[3];
    DWORD ids[3];

    threads[0] = CreateThread(NULL, 0, set_number, NULL, 0, &ids[0]);
    threads[1] = CreateThread(NULL, 0, get_number, NULL, 0, &ids[1]);
    threads[2] = CreateThread(NULL, 0, buffer_control, NULL, 0, &ids[2]);

    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    for (int i = 0; i < 3; i++) {
        CloseHandle(threads[i]);
    }

    CloseHandle(g_mutex);  // закрытие мьютекса

    return 0;
}
