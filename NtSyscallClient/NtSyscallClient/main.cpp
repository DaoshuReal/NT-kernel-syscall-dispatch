#include <windows.h>
#include <stdio.h>

typedef int(NTAPI* NtMITAccessibilityTimerNotification)(int input);

int main()
{
    LoadLibraryA("user32.dll");
    HMODULE hWin32u = GetModuleHandleA("win32u.dll");
    if (!hWin32u)
    {
        printf("[-] Failed to load win32u.dll: %d\n", GetLastError());
        return 1;
    }

    printf("[+] win32u.dll loaded at: 0x%p\n", hWin32u);

    NtMITAccessibilityTimerNotification pFunc =
        (NtMITAccessibilityTimerNotification)GetProcAddress(hWin32u, "NtMITAccessibilityTimerNotification");

    if (!pFunc)
    {
        printf("[-] Failed to find NtMITAccessibilityTimerNotification: %d\n", GetLastError());
        FreeLibrary(hWin32u);
        return 1;
    }

    printf("[+] NtMITAccessibilityTimerNotification at: 0x%p\n", pFunc);
    printf("[+] Calling function with input 10\n");

    int output = pFunc(10);

    printf("[+] Function returned: %d\n", output);

    FreeLibrary(hWin32u);
    return 0;
}
