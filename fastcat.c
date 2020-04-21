#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>

/* Removes the O_APPEND flag from a given fd */
static void remove_append(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    if (flags >= 0)
        fcntl(fd, F_SETFL, flags ^ O_APPEND);
}

/* Returns true if STDOUT_FILENO has O_APPEND set; false otherwise */
static bool stdout_append()
{
    return fcntl(STDOUT_FILENO, F_GETFL) & O_APPEND;
}

int main(int argc, char **argv)
{
    if (argc < 2) { /* No file was given. */
        fprintf(stderr, "Usage: %s FILE [FILE...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (stdout_append()) {
        /* In some cases, stdout can have O_APPEND set but not be pointing
         * to a particular file.
         * FIXME: It is not safe to remove O_APPEND when STDOUT_FILENO is not
         *        a tty. We should not overwrite files. That is, we have to
         *        add "&& isatty(STDOUT_FILENO)" for the above if-statement.
         */
        remove_append(STDOUT_FILENO);
    }

    /* Neither splice nor sendfile support file descriptors with
     * O_APPEND. Either removing O_APPEND failed or we're not redirected to
     * a tty.
     */
    if (stdout_append()) {
        fprintf(stderr, "Unable to append to files.\n");
        return EXIT_FAILURE;
    }

    int num_files = argc - 1;
    int fds[num_files];
    struct stat stats[num_files];

    /* Open all files and add them to the array of file descriptors. */
    for (int i = 0; i < num_files; i++) {
        char *in_filename = argv[i + 1];
        fds[i] = open(in_filename, O_RDONLY);
        if (fds[i] < 0) {
            fprintf(stderr, "Unable to open %s.\n", in_filename);
            if (errno == ENOENT)
                fprintf(stderr, "File does not exist.\n");
            return EXIT_FAILURE;
        }

        fstat(fds[i], (stats + i));
        if (S_ISDIR(stats[i].st_mode)) {
            fprintf(stderr, "%s is a directory.\n", in_filename);
            return EXIT_FAILURE;
        }
    }

    /* Copy the files to stdout. Close the files when done. Exit with a
     * non-zero status on error.
     */
    for (int i = 0; i < num_files; i++) {
        /* Copy between the file descriptors without an intermediate copy into
         * userspace. Try splice in case one is a pipe (which stdout often is);
         * if not, then use sendfile if splice set errno to EINVAL (as it does
         * when neither argument is a pipe).
         */
        ssize_t bytes =
            splice(fds[i], NULL, STDOUT_FILENO, NULL, stats[i].st_size, 0);
        if (bytes == -1 && errno == EINVAL)
            bytes = sendfile(STDOUT_FILENO, fds[i], NULL, stats[i].st_size);

        /* If things still failed, exit with errno as the exit status. */
        if (bytes < 0)
            return errno;
        close(fds[i]);
    }

    return EXIT_SUCCESS;
}
