#include <windows.h>

#define BUFFER_LEN 4096

int main(void) {
    WCHAR wstrBuff[BUFFER_LEN];
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwLen = 1;

    while (ReadConsoleW(hStdIn, wstrBuff, BUFFER_LEN, &dwLen, NULL) && dwLen > 0) {
        if (! WriteConsoleW(hStdOut, wstrBuff, dwLen, NULL, NULL)) break;
    }
    return dwLen != 0;
}