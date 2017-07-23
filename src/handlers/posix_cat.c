/**********************************************************************
NAME

    cat - concatenate and print files

SYNOPSIS

    cat [-u] [file...]

DESCRIPTION

	The cat utility shall read files in sequence and shall write their contents
    to the standard output in the same sequence.

OPTIONS

    The cat utility shall conform to XBD Utility Syntax Guidelines .

    The following option shall be supported:

	-u		Write bytes from the input file to the standard output without
			delay as each is read.

OPERANDS

    The following operand shall be supported:

	file 	A pathname of an input file. If no file operands are specified, the
			standard input shall be used. If a file is '-', the cat utility
			shall read from the standard input at that point in the sequence.
			The cat utility shall not close and reopen standard input when it
			is referenced in this way, but shall accept multiple occurrences of
			'-' as a file operand.

STDIN

	The standard input shall be used only if no file operands are specified, or
	if a file operand is '-'. See the INPUT FILES section.

INPUT FILES

    The input files can be any file type.

ENVIRONMENT VARIABLES

    The following environment variables shall affect the execution of cat:

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
        [XSI] [Option Start] Determine the location of message catalogs for the
        processing of LC_MESSAGES. [Option End]

ASYNCHRONOUS EVENTS

    Default.

STDOUT

    The standard output shall contain the sequence of bytes read from the input
    files. Nothing else shall be written to the standard output. If the
    standard output is a regular file, and is the same file as any of the input
    file operands, the implementation may treat this as an error.

STDERR

    The standard error shall be used only for diagnostic messages.

OUTPUT FILES

    None.

EXTENDED DESCRIPTION

    None.

EXIT STATUS

    The following exit values shall be returned:

     0
        All input files were output successfully.
    >0
        An error occurred.

CONSEQUENCES OF ERRORS

    Default.

 **********************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "handlers.h"

#define PROGRAM     "cat"

/*
 * Single page of memory that is used to read from FILE/stdin and write to
 * stdout. However, this is unused if cat is unbuffered.
 */
char *buffer_page;

/* Size of the buffer page */
size_t buffer_size;

/* Flag to indicate if I/O should be unbuffered */
int unbuffered;

static void usage(void)
{
    fprintf(stderr, "Usage: %s [-u] [file...]\n", PROGRAM);
}

static int cat_file(char *filename)
{
    int fd = -1;
    int close_fd = 0;
    int bytes_read;
    int bytes_written;
    int retval = 0;

    if (strcmp(filename, "-") == 0) {
        /* Output stdin */
        fd = STDIN_FILENO;
    } else {
        fd = open(filename, O_RDONLY);
        
        if (fd == -1) {
            fprintf(stderr, "%s: %s: %s\n", PROGRAM, filename, strerror(errno));
            return 1;
        }
        
        close_fd = 1;
    }

    for (;;) {
        bytes_read = read(fd, buffer_page, buffer_size);
        if (bytes_read == 0) break;
        if (bytes_read == -1) {
            /* If the read failed because it was interrupted by a signal,
             * then simply try again.
             */
            if (errno == EINTR) continue;
            fprintf(stderr, "%s: %s: %s\n", PROGRAM, filename, strerror(errno));
            retval = 1;
            break;
        }

        bytes_written = write(STDOUT_FILENO, buffer_page, buffer_size);
        if (bytes_written == -1) {
            fprintf(stderr, "%s: stdout: %s\n", PROGRAM, strerror(errno));
            retval = 1;
            break;
        }
    }

    if (close_fd)
        close(fd);

    return retval; 
}

int posix_cat(int argc, char **argv)
{
    int opt;
    int retval = 0;
    char **file;
    int call_free = 0;
    char buffer;

    unbuffered = 0;
    /* Parse arguments */
    while ((opt = getopt(argc, argv, "u")) != -1) {
        switch (opt) {
        case 'u':
            unbuffered = 1;
            break;
        default:
            usage();
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (unbuffered) {
        setbuf(stdout, NULL);
        buffer_page = &buffer;
        buffer_size = 1;
    } else {
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
    }

    if ((argc - optind) == 0) {
        /*
         * No additional arguments specified, handle it as if a single '-'
         * was provided for the input
         */
        return cat_file("-");
    }

    for (file = &argv[optind]; *file; file++) {
        if (cat_file(*file)) {
            retval = 1;
        }
    }

    if (call_free) {
        free(buffer_page);
    } else {
        munmap(buffer_page, buffer_size);
    }
    return retval;
}
