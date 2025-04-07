#include <Windows.h>
#include <stdio.h>

#define MAX_TIMES 100

DWORD g_dwtimes[MAX_TIMES];
int g_nindex = 0;

HANDLE g_mutex;

// Поток записи
DWORD WINAPI writer_thread(LPVOID param) {
    while (1) {
        if (WaitForSingleObject(g_mutex, INFINITE) == WAIT_OBJECT_0) {
            if (g_nindex >= MAX_TIMES) {
                ReleaseMutex(g_mutex);
                break;
            }

            g_dwtimes[g_nindex] = GetTickCount();
            g_nindex++;

            ReleaseMutex(g_mutex);
            Sleep(10);
        } else {
            break;
        }
    }
    return 0;
}

DWORD WINAPI writer_thread2(LPVOID param) {
    while (1) {
        if (WaitForSingleObject(g_mutex, INFINITE) == WAIT_OBJECT_0) {
            if (g_nindex >= MAX_TIMES - 1) {
                ReleaseMutex(g_mutex);
                break;
            }

            g_nindex++;
            g_dwtimes[g_nindex] = GetTickCount();

            ReleaseMutex(g_mutex);
            Sleep(10);
        } else {
            break;
        }
    }
    return 0;
}

int main() {
    g_mutex = CreateMutex(NULL, FALSE, NULL);

    HANDLE threads[2];
    threads[0] = CreateThread(NULL, 0, writer_thread, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 0, writer_thread2, NULL, 0, NULL);

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
    CloseHandle(g_mutex);

    for (int i = 0; i < MAX_TIMES; i++) {
        printf("%lu ", g_dwtimes[i]);
        if ((i + 1) % 10 == 0) printf("\n");
    }

    return 0;
}
