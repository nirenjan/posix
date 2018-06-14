/**********************************************************************
NAME

    tee - duplicate standard input

SYNOPSIS

    tee [-ai] [file...]

DESCRIPTION

    The tee utility shall copy standard input to standard output, making a copy
    in zero or more files. The tee utility shall not buffer output.

    If the -a option is not specified, output files shall be written (see File
    Read, Write, and Creation.

OPTIONS

    The tee utility shall conform to XBD Utility Syntax Guidelines .

    The following options shall be supported:

    -a
        Append the output to the files.
    -i
        Ignore the SIGINT signal.

OPERANDS

    The following operands shall be supported:

    file
        A pathname of an output file. If a file operand is '-', it shall refer
        to a file named -; implementations shall not treat it as meaning
        standard output. Processing of at least 13 file operands shall be
        supported.

STDIN

    The standard input can be of any type.

INPUT FILES

    None.

ENVIRONMENT VARIABLES

    The following environment variables shall affect the execution of tee:

    LANG
        Provide a default value for the internationalization variables that are
        unset or null. (See XBD Internationalization Variables for the
        precedence of internationalization variables used to determine the
        values of locale categories.)
    LC_ALL
        If set to a non-empty string value, override the values of all the
        other internationalization variables.
    LC_CTYPE
        Determine the locale for the interpretation of sequences of bytes of
        text data as characters (for example, single-byte as opposed to
        multi-byte characters in arguments).
    LC_MESSAGES
        Determine the locale that should be used to affect the format and
        contents of diagnostic messages written to standard error.
    NLSPATH
        [XSI] Determine the location of message catalogs for the processing of
        LC_MESSAGES.

ASYNCHRONOUS EVENTS

    Default, except that if the -i option was specified, SIGINT shall be
    ignored.

STDOUT

    The standard output shall be a copy of the standard input.

STDERR

    The standard error shall be used only for diagnostic messages.

OUTPUT FILES

    If any file operands are specified, the standard input shall be copied to
    each named file.

EXTENDED DESCRIPTION

    None.

EXIT STATUS

    The following exit values shall be returned:

     0
        The standard input was successfully copied to all output files.
    >0
        An error occurred.

CONSEQUENCES OF ERRORS

    If a write to any successfully opened file operand fails, writes to other
    successfully opened file operands and standard output shall continue, but
    the exit status shall be non-zero. Otherwise, the default actions specified
    in Utility Description Defaults apply.

 **********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PROGRAM     "tee"

static void usage(void)
{
    fprintf(stderr, "Usage: %s [-ai] [file...]\n", PROGRAM);
}

int posix_tee(int argc, char **argv)
{
    int opt;
    int i;
    int retval = 0;
    char *file;
    int call_free = 0;
    char buffer;
    int tee_files;
    int *fds = NULL;
    int open_flags = O_TRUNC;

    char *buffer_page;
    size_t buffer_size;

    int bytes_read;
    int bytes_written;

    /* Parse arguments */
    while ((opt = getopt(argc, argv, "ai")) != -1) {
        switch (opt) {
        case 'a':
            open_flags = O_APPEND;
            break;
        case 'i':
            signal(SIGINT, SIG_IGN);
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
            break;
        }
    }

    buffer_size = sysconf(_SC_PAGE_SIZE);
    buffer_page = mmap(NULL, buffer_size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer_page == MAP_FAILED) {
        fprintf(stderr, "%s: %s\n", PROGRAM, strerror(errno));
        
        /* mmap failed, but try to malloc instead */
        buffer_page = malloc(buffer_size);
        if (buffer_page == NULL) {
            fprintf(stderr, "%s: %s\n", PROGRAM, strerror(errno));
            exit(EXIT_FAILURE);
        }
        call_free = 1;
    }
    setbuf(stdout, NULL);

    tee_files = argc - optind;
    if (tee_files != 0) {
        /* Allocate an array for the file descriptors */
        fds = calloc(sizeof(int), tee_files);

        if (fds == NULL) {
            fprintf(stderr, "%s: %s\n", PROGRAM, strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* Open files for writing */
        for (int i = 0; i < tee_files; i++) {
            file = argv[optind + i];
            fds[i] = open(file, O_CREAT | O_WRONLY | open_flags,
                          S_IRWXU | S_IRWXG | S_IRWXO);

            if (fds[i] == -1) {
                fprintf(stderr, "%s: %s: %s\n", PROGRAM, file, strerror(errno));
                retval = 1;
            }
        }
    }

    /* Read from stdin and write to stdout, followed by additional files */
    for (;;) {
        bytes_read = read(STDIN_FILENO, buffer_page, buffer_size);
        if (bytes_read == 0) break;
        if (bytes_read == -1) {
            /* If the read failed because it was interrupted by a signal,
             * then simply try again.
             */
            if (errno == EINTR) continue;
            fprintf(stderr, "%s: stdin: %s\n", PROGRAM, strerror(errno));
            retval = 1;
            break;
        }

        bytes_written = write(STDOUT_FILENO, buffer_page, buffer_size);
        if (bytes_written == -1) {
            fprintf(stderr, "%s: stdout: %s\n", PROGRAM, strerror(errno));
            retval = 1;
            break;
        }

        for (i = 0; i < tee_files; i++) {
            /* Write to each of the tee files */
            bytes_written = write(fds[i], buffer_page, buffer_size);
            if (bytes_written == -1) {
                fprintf(stderr, "%s: %s: %s\n", PROGRAM, argv[optind + i],
                        strerror(errno));
                retval = 1;
            }
        }
    }

    /* Close all opened files */
    for (i = 0; i < tee_files; i++) {
        close(fds[i]);
    }

    free(fds);

    if (call_free) {
        free(buffer_page);
    } else {
        munmap(buffer_page, buffer_size);
    }
    return retval;
}
