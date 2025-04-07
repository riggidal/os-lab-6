#include <stdio.h>
#include <Windows.h>

HANDLE event;

int main()
{    
    event = OpenEvent(EVENT_MODIFY_STATE, 1, TEXT("Global\\symbol_is_handled"));

    while (1)
    {
        getc(stdin);
        SetEvent(event);
    }
}