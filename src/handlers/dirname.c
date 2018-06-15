/**********************************************************************
NAME

    dirname - return the directory portion of a pathname

SYNOPSIS

    dirname string

DESCRIPTION

    The string operand shall be treated as a pathname, as defined in XBD
    Pathname. The string string shall be converted to the name of the directory
    containing the filename corresponding to the last pathname component in
    string, performing actions equivalent to the following steps in order:

    1.  If string is //, skip steps 2 to 5.

    2.  If string consists entirely of <slash> characters, string shall be set
        to a single <slash> character. In this case, skip steps 3 to 8.

    3.  If there are any trailing <slash> characters in string, they shall be
        removed.

    4.  If there are no <slash> characters remaining in string, string shall be
        set to a single <period> character. In this case, skip steps 5 to 8.

    5.  If there are any trailing non- <slash> characters in string, they shall
        be removed.

    6.  If the remaining string is //, it is implementation-defined whether
        steps 7 and 8 are skipped or processed.

    7.  If there are any trailing <slash> characters in string, they shall be
        removed.

    8.  If the remaining string is empty, string shall be set to a single
        <slash> character.

    The resulting string shall be written to standard output.

OPTIONS

    None.

OPERANDS

    The following operand shall be supported:

    string
        A string.

STDIN

    Not used.

INPUT FILES

    None.

ENVIRONMENT VARIABLES

    The following environment variables shall affect the execution of dirname:

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

    Default.

STDOUT

    The dirname utility shall write a line to the standard output in the
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

int posix_dirname(int argc, char **argv)
{
    int i;
    int only_slashes;
    char *string_ptr;
    char *tmp_ptr;
    size_t string_len;

    if (argc != 2) {
        fprintf(stderr, "Usage: dirname string\n");
        return 1;
    }

    string_ptr = strdup(argv[1]);
    string_len = strlen(argv[1]);

    setbuf(stdout, NULL);
step1:
    /* Check if string is // */
    if (strcmp(string_ptr, "//") == 0) {
        goto step6;
    }

step2:
    /* Check if the string consists of only slashes */
    for (tmp_ptr = argv[1], only_slashes = 1; *tmp_ptr; tmp_ptr++) {
        if (*tmp_ptr != '/') {
            only_slashes = 0;
            break;
        }
    }

    if (only_slashes) {
        string_ptr[1] = '\0';
        goto output;
    }

step3:
    /* Step 3, remove any trailing slashes */
    for (i = string_len - 1; i >= 0; i++) {
        if (string_ptr[i] == '/') {
            string_ptr[i] = '\0';
        } else {
            break;
        }
    }

    string_len = strlen(string_ptr);

step4:
    /*
     * Check for any slash characters left in the string.
     * If none, set to single period character
     */
    if (strchr(string_ptr, '/') == NULL) {
        free(string_ptr);
        puts(".");
        return 0;
    }

step5:
    /* Step 5, remove any trailing non-slashes */
    for (i = string_len - 1; i >= 0; i--) {
        if (string_ptr[i] != '/') {
            string_ptr[i] = '\0';
        } else {
            break;
        }
    }

    string_len = strlen(string_ptr);

step6:
    /* If remaining string is //, implementation defined to process steps 7,8 */
    /* IMPLEMENTATION SPECIFIC: Process 7 & 8 regardless of string */
step7:
    /* Remove trailing slash characters */
    for (i = string_len - 1; i >= 0; i++) {
        if (string_ptr[i] == '/') {
            string_ptr[i] = '\0';
        } else {
            break;
        }
    }

    string_len = strlen(string_ptr);

step8:
    /* If string is empty, set it to "/" */
    if (string_len == 0) {
        string_ptr[0] = '/';
        string_len = 1;
    }

output:
    puts(string_ptr);
    free(string_ptr);
    return 0;
}
