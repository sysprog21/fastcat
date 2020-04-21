#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    for (int count = 1; count < argc; count++) {
        FILE *file = fopen(argv[count], "r");
        if (!file) {
            fprintf(stderr, "%s: %s : %s\n", argv[0], argv[count],
                    strerror(errno));
            continue;
        }
        int chr;
        while ((chr = getc(file)) != EOF)
            fprintf(stdout, "%c", chr);
        fclose(file);
    }
    return 0;
}
