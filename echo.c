#include <windows.h>

int main(void) {
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdIn == INVALID_HANDLE_VALUE || hStdOut == INVALID_HANDLE_VALUE) return 1;

    CHAR ch;
    DWORD dwRead = 1;
    DWORD dwWritten;
    while (ReadFile(hStdIn, &ch, 1, &dwRead, NULL) && dwRead != 0) {
        WriteFile(hStdOut, &ch, dwRead, &dwWritten, NULL);
        if (dwRead != dwWritten) break;
    }

    return 0;
}