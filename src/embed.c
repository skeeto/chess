#include <stdio.h>

int main(void)
{
    #ifdef _WIN32
    int _setmode(int, int);
    _setmode(0, 0x8000);
    #endif
    for (int i = 0, c = getchar(); c != EOF; i++, c = getchar()) {
        printf("0x%02x,%c", c, i%12 == 11 ? '\n' : ' ');
    }
    fflush(stdout);
    return ferror(stdin) || ferror(stdout);
}
