#include <windows.h>

#define LINE_BUFF_LEN 4096
#define LF '\n'

typedef struct {
    DWORD dwOffsetForRead;
    DWORD dwOffsetForWrite;
    CHAR csLineBuff[LINE_BUFF_LEN];
} MovableBuff;

void InitializeBuff(MovableBuff* pBuff) {
    pBuff->dwOffsetForRead = 0;
    pBuff->dwOffsetForWrite = 0;
}

void WriteNumberString(CHAR* pStr, DWORD dwLen, DWORD dwValue, DWORD dwBase) {
    static const CHAR chNumbers[] = {"0123456789ABCDEF"};
    CHAR cZeroOrSpace = '0';
    for (int i = dwLen - 1; i >= 0; --i) {
        pStr[i] = (dwValue == 0) ? cZeroOrSpace : chNumbers[dwValue % dwBase];
        dwValue /= dwBase;
        cZeroOrSpace = (dwBase == 10) ? ' ' : '0';
    }
}

int ReadBuff(HANDLE h, MovableBuff* pBuff) {
    if (LINE_BUFF_LEN == pBuff->dwOffsetForRead) return -1;
    DWORD dwLen = 0;
    BOOL result = ReadFile(h, &pBuff->csLineBuff[pBuff->dwOffsetForRead], LINE_BUFF_LEN - pBuff->dwOffsetForRead, &dwLen, NULL);
    if (! result) return -1;
    if (dwLen == 0) return 0;
    pBuff->dwOffsetForRead += dwLen;
#if defined(VERBOSE)
#define HEAD "dwLength: "
#define LSLEN(x) (sizeof(x)-1)
    CHAR outbuf[] = {HEAD "4294967295\r\n"};
    WriteNumberString(&outbuf[LSLEN(HEAD)], 10, dwLen, 10);
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), outbuf, LSLEN(outbuf), NULL, NULL);
#endif
    return 1;
}

BOOL NextLine(MovableBuff* pBuff, CHAR** pStr, DWORD* pLen) {
    DWORD i = pBuff->dwOffsetForWrite;
    for (; i < pBuff->dwOffsetForRead; ++i) {
        if (pBuff->csLineBuff[i] == LF) break;
    }
    if (i == pBuff->dwOffsetForRead) return FALSE;
    ++i;
    *pStr = &pBuff->csLineBuff[pBuff->dwOffsetForWrite];
    *pLen = i - pBuff->dwOffsetForWrite;
    pBuff->dwOffsetForWrite = i;
    return TRUE;
}

void MoveWritten(MovableBuff* pBuff) {
    DWORD restlen = pBuff->dwOffsetForRead - pBuff->dwOffsetForWrite;
    if (restlen == 0) {
        pBuff->dwOffsetForRead = pBuff->dwOffsetForWrite = 0;
    } else if (pBuff->dwOffsetForWrite > 0) {
        MoveMemory(pBuff->csLineBuff, pBuff->csLineBuff + pBuff->dwOffsetForWrite, pBuff->dwOffsetForRead - pBuff->dwOffsetForWrite);
        pBuff->dwOffsetForRead -= pBuff->dwOffsetForWrite;
        pBuff->dwOffsetForWrite = 0;
    }
}

BOOL IsFilled(MovableBuff* pBuff) {
    return pBuff->dwOffsetForRead == LINE_BUFF_LEN && pBuff->dwOffsetForWrite == 0;
}

DWORD NextAll(MovableBuff* pBuff, CHAR** pStr) {
    *pStr = &pBuff->csLineBuff[pBuff->dwOffsetForWrite];
    DWORD dwLen = pBuff->dwOffsetForRead - pBuff->dwOffsetForWrite;
    pBuff->dwOffsetForWrite = pBuff->dwOffsetForRead = 0;
    return dwLen;
}

#define BYTES_PER_LINE 16

BOOL WriteHex(HANDLE h, CHAR* s, DWORD dwLen) {
    DWORD dwLineCount = (dwLen + BYTES_PER_LINE - 1) / BYTES_PER_LINE;
    CHAR line[BYTES_PER_LINE * 3 + 2];
    CHAR* p = line; 
    for (DWORD i = 0; i < dwLen; ++i) {
        WriteNumberString(p, 2, (DWORD)(unsigned char)s[i], 16);
        p += 2;
        *p++ = ' ';
        if (((i+1) % BYTES_PER_LINE == 0) || (i+1 == dwLen)) {
            *p++ = '\r';
            *p++ = '\n';
            DWORD dwWritten = 0;
            WriteFile(h, line, p - line, &dwWritten, NULL);
            if (p - line != dwWritten) return FALSE;
            p = line;
        }
    }
    return TRUE;
}

BOOL WriteData(HANDLE h, CHAR* s, DWORD dwLen) {
    DWORD dwWritten = 0;
    WriteFile(h, s, dwLen, &dwWritten, NULL);
    WriteHex(h, s, dwLen);
    return dwLen == dwWritten;
}

int main(void) {
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdIn == INVALID_HANDLE_VALUE || hStdOut == INVALID_HANDLE_VALUE) return 1;

    MovableBuff buff; 
    InitializeBuff(&buff);
    int res;
    CHAR* str = NULL;
    DWORD len = 0;
    while ((res = ReadBuff(hStdIn, &buff)) > 0) {
        while (NextLine(&buff, &str, &len)) {
            if (! WriteData(hStdOut, str, len)) return 1;
        }
        MoveWritten(&buff);
        if (! IsFilled(&buff)) continue;
        if ((len = NextAll(&buff, &str)) == 0) continue;
        if (! WriteData(hStdOut, str, len)) return 1;
    }
    if ((len = NextAll(&buff, &str)) > 0) {
        if (! WriteData(hStdOut, str, len)) return 1;
    }

    return res != 0;
}
