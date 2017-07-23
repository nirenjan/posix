/**********************************************************************
NAME

    basename - return non-directory portion of a pathname

SYNOPSIS

    basename string [suffix]

DESCRIPTION

    The string operand shall be treated as a pathname, as defined in XBD
    Pathname. The string string shall be converted to the filename
    corresponding to the last pathname component in string and then the suffix
    string suffix, if present, shall be removed. This shall be done by
    performing actions equivalent to the following steps in order:

    1.  If string is a null string, it is unspecified whether the resulting
        string is '.' or a null string. In either case, skip steps 2 through 6.

    2.  If string is "//", it is implementation-defined whether steps 3 to 6
        are skipped or processed.

    3.  If string consists entirely of <slash> characters, string shall be set
        to a single <slash> character. In this case, skip steps 4 to 6.

    4.  If there are any trailing <slash> characters in string, they shall be removed.

    5.  If there are any <slash> characters remaining in string, the prefix of
        string up to and including the last <slash> character in string shall
        be removed.

    6.  If the suffix operand is present, is not identical to the characters
        remaining in string, and is identical to a suffix of the characters
        remaining in string, the suffix suffix shall be removed from string.
        Otherwise, string is not modified by this step. It shall not be
        considered an error if suffix is not found in string.

    The resulting string shall be written to standard output.

OPTIONS

    None.

OPERANDS

    The following operands shall be supported:

    string
        A string.
    suffix
        A string.

STDIN

    Not used.

INPUT FILES

    None.

ENVIRONMENT VARIABLES

    The following environment variables shall affect the execution of basename:

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

    The basename utility shall write a line to the standard output in the
    following format:

    "%s\n", <resulting string>

STDERR

    The standard error shall be used only for diagnostic messages.

OUTPUT FILES

    None.

EXTENDED DESCRIPTION

    None.

EXIT STATUS

    The following exit values shall be returned:

     0
        Successful completion.
    >0
        An error occurred.

CONSEQUENCES OF ERRORS

    Default.

 **********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int posix_basename(int argc, char **argv)
{
    int i;
    int only_slashes;
    char *string_ptr;
    char *out_ptr;
    size_t string_len;
    size_t suffix_len;
    size_t suffix_pos = 0;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: basename string [suffix]\n");
        return 1;
    }

    /* At least 2 arguments, check if the string consists of only slashes */
    for (string_ptr = argv[1], only_slashes = 1; *string_ptr; string_ptr++) {
        if (*string_ptr != '/') {
            only_slashes = 0;
            break;
        }
    }

    if (only_slashes) {
        /* Step 3 - if only slashes, print the last slash and exit */
        /* Go back one step, and print the string */
        string_ptr--;
        printf("%s\n", string_ptr);
        return 0;
    }

    /* Step 4, remove any trailing slashes */
    string_len = strlen(argv[1]);
    string_ptr = strdup(argv[1]);

    for (i = string_len - 1; i >= 0; i++) {
        if (string_ptr[i] == '/') {
            string_ptr[i] = '\0';
            string_len--;
        } else {
            break;
        }
    }

    /* Step 5, remove up to last / */
    out_ptr = strrchr(string_ptr, '/');
    if (out_ptr == NULL) {
        out_ptr = string_ptr;
    } else {
        out_ptr++;
    }
    string_len = strlen(out_ptr);

    /* Step 6, remove a trailing suffix, if it exists */
    if (argc == 3) {
        suffix_len = strlen(argv[2]);

        if (string_len > suffix_len) {
            suffix_pos = string_len - suffix_len;
            if (strcmp(&out_ptr[suffix_pos], argv[2]) == 0) {
                /* Suffix matches, get rid of it */
                out_ptr[suffix_pos] = 0;
            }
        }
    }

    printf("%s\n", out_ptr);

    free(string_ptr);
    return 0;
}
