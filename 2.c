#include <stdio.h>
#include <Windows.h>

#define MAX_TIMES 100

int g_nindex = 0;
DWORD g_dwtimes[MAX_TIMES];
CRITICAL_SECTION g_cs; // для синхронизации доступа

DWORD WINAPI func_one(LPVOID lpParam)
{
    while (1)
    {
        EnterCriticalSection(&g_cs);
        if (g_nindex >= MAX_TIMES)
        {
            LeaveCriticalSection(&g_cs);
            break;
        }
        g_dwtimes[g_nindex++] = GetTickCount();
        LeaveCriticalSection(&g_cs);
        Sleep(10);
    }
    return 0;
}

DWORD WINAPI func_two(LPVOID lpParam)
{
    while (1)
    {
        EnterCriticalSection(&g_cs);
        if (g_nindex >= MAX_TIMES)
        {
            LeaveCriticalSection(&g_cs);
            break;
        }
        g_dwtimes[g_nindex++] = GetTickCount();
        LeaveCriticalSection(&g_cs);
        Sleep(10);
    }
    return 0;
}

int main()
{
    HANDLE threads[2];
    DWORD ids[2];

    InitializeCriticalSection(&g_cs); // инициализация критической секции

    threads[0] = CreateThread(NULL, 0, func_one, NULL, 0, &ids[0]);
    threads[1] = CreateThread(NULL, 0, func_two, NULL, 0, &ids[1]);

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    CloseHandle(threads[0]);
    CloseHandle(threads[1]);

    DeleteCriticalSection(&g_cs); // освобождение

    for (int i = 0; i < MAX_TIMES; i++)
    {
        printf("%u ", g_dwtimes[i]);
        if ((i + 1) % 10 == 0)
            printf("\n");
    }

    return 0;
}
