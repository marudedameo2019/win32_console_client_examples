#include <windows.h>

#define LSLEN(x) (sizeof(x)-1)
#define OUTPUT(h, s) Output(h, s, LSLEN(s))

BOOL Output(HANDLE h, CHAR* s, DWORD l) {
    return WriteFile(h, s, l, NULL, NULL);
}

int main(void) {
    CHAR strStdin[]   = {"stdin:  "};
    CHAR strStdout[]  = {"stdout: "};
    CHAR strConsole[] = {"console(or character device)"};
    CHAR strOther[]   = {"other"};
    CHAR strLF[]      = {"\n"};

    HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
    OUTPUT(hStdOut, strStdin);
    if (GetFileType(hStdIn) == FILE_TYPE_CHAR) {
        OUTPUT(hStdOut, strConsole);
    } else {
        OUTPUT(hStdOut, strOther);
    }
    OUTPUT(hStdOut, strLF);
        
    OUTPUT(hStdOut, strStdout);
    if (GetFileType(hStdOut) == FILE_TYPE_CHAR) {
        OUTPUT(hStdOut, strConsole);
    } else {
        OUTPUT(hStdOut, strOther);
    }
    OUTPUT(hStdOut, strLF);

    return 0;
}