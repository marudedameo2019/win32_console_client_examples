#include <Windows.h>

#define LSLEN(x) (sizeof(x)-1)

void WriteNumberString(CHAR* pStr, DWORD dwLen, DWORD dwValue, DWORD dwBase) {
    static const CHAR chNumbers[] = {"0123456789ABCDEF"};
    CHAR cZeroOrSpace = '0';
    for (int i = dwLen - 1; i >= 0; --i) {
        pStr[i] = (dwValue == 0) ? cZeroOrSpace : chNumbers[dwValue % dwBase];
        dwValue /= dwBase;
        cZeroOrSpace = (dwBase == 10) ? ' ' : '0';
    }
}

#define VALUE_PLUS(ch, value, min, max, offset) ((ch >= min && ch <= max) ? ((value += ch - min + offset), TRUE) : FALSE)

BOOL GetNumberFromString(CHAR* szStr, DWORD dwBase, DWORD* pdwValue) {
    static const CHAR chNumbers[] = {"0123456789ABCDEF"};
    if (szStr == NULL) return FALSE;
    if (dwBase != 8 && dwBase != 10 && dwBase != 16) return FALSE;
    DWORD value = 0;
    BOOL result = TRUE;
    for (int i = 0; result && szStr[i] != '\0' && i < 11; ++i) { // care security!
        value *= dwBase;
        CHAR ch = szStr[i];
        result = FALSE;
        result = result || VALUE_PLUS(ch, value, '0', '9', 0);
        result = result || ((dwBase == 16) ? VALUE_PLUS(ch, value, 'A', 'F', 10) : FALSE);
        result = result || ((dwBase == 16) ? VALUE_PLUS(ch, value, 'a', 'f', 10) : FALSE);
    }
    if (result) *pdwValue = value;
    return result;
}

int main(int argc, char* argv[]) {
    if (argc > 2) {
        DWORD cp;
        if (! GetNumberFromString(argv[1], 10, &cp)) return 1;
        if (! SetConsoleCP(cp)) return 1;
        if (! GetNumberFromString(argv[2], 10, &cp)) return 1;
        if (! SetConsoleOutputCP(cp)) return 1;
    }
    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CHAR strInputCP[] =  {"GetConsoleCP():      "};
    CHAR strOutputCP[] = {"GetConsoleOutputCP():"};
    CHAR strLF[] = {"\n"};
    CHAR strNumber[] = {"4294967295"};

    WriteFile(hStdOut, strInputCP, LSLEN(strInputCP), NULL, NULL);
    WriteNumberString(strNumber, LSLEN(strNumber), GetConsoleCP(), 10);
    WriteFile(hStdOut, strNumber, LSLEN(strNumber), NULL, NULL);
    WriteFile(hStdOut, strLF, LSLEN(strLF), NULL, NULL);

    WriteFile(hStdOut, strOutputCP, LSLEN(strOutputCP), NULL, NULL);
    WriteNumberString(strNumber, LSLEN(strNumber), GetConsoleOutputCP(), 10);
    WriteFile(hStdOut, strNumber, LSLEN(strNumber), NULL, NULL);
    WriteFile(hStdOut, strLF, LSLEN(strLF), NULL, NULL);

    return 0;
}