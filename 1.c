#include <stdio.h>
#include <Windows.h>

#define MAX_TIMES 100

int g_nindex = 0;

DWORD g_dwtimes[MAX_TIMES];

DWORD WINAPI func_one()
{
    while (g_nindex < MAX_TIMES)
    {
        g_dwtimes[g_nindex] = GetTickCount();
        Sleep(10);
        g_nindex++;
    }
    return 0;
}

DWORD WINAPI func_two()
{
    while (g_nindex < MAX_TIMES)
    {
        g_nindex++;
        g_dwtimes[g_nindex] = GetTickCount();
        Sleep(10);
    }
    return 0;
}

int main()
{
    HANDLE threads[2];
    DWORD ids[2];

    threads[0] = CreateThread(NULL, 0, func_one, NULL, 0, &ids[0]);
    threads[1] = CreateThread(NULL, 0, func_two, NULL, 0, &ids[1]);

    WaitForMultipleObjects(2, threads, 1, INFINITE);

    CloseHandle(threads[0]);
    CloseHandle(threads[1]);

    for (int i=0; i<MAX_TIMES; i++)
    {
        printf("%d ", g_dwtimes[i]);
        if (!((i+1)%10) && i)
            printf("\n");
    }
}
