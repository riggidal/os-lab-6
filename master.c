#include <stdio.h>
#include <Windows.h>

HANDLE event;

int main()
{
    // Событие с чтением данных ещё не произошло, инициализируем 0
    event = CreateEvent(NULL, 1, 0, "Global\\symbol_is_handled");
    while (1)
    {
        WaitForSingleObject(event, INFINITE);
        putc('*', stdout);
        ResetEvent(event);
    }

    CloseHandle(event);
}