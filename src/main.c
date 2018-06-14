#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

typedef int (*handler_function)(int, char**);

/*
 * Main function which dispatches the execution to different handlers
 * depending on the value of argv[0]
 */
int main(int argc, char **argv)
{
    void *command_interp;
    int retval = 0;
    char *command;
    char command_func[64];
    handler_function handler;
    int offset = 0;

    /* Get the basename of the command */
    command = strrchr(argv[0], '/');
    if (command == NULL) {
        command = argv[0];
    } else {
        /* Skip over the / */
        command++;
    }

    /* If the command matches PROGNAME, then use argv[1] instead */
    if (strcmp(command, PROGNAME) == 0) {
        command = argv[1];
        offset = 1;
    }

    /* Load the current executable to lookup the symbol */
    command_interp = dlopen(NULL, RTLD_NOW);
    if (command_interp == NULL) {
        fprintf(stderr, "Unable to load interpreter: %s\n", strerror(errno));
        return 1;
    }

    /* Generate the function name for the executable */
    snprintf(command_func, sizeof(command_func), "posix_%s", command);

    /* Lookup the function */
    handler = dlsym(command_interp, command_func);

    if (!handler) {
        fprintf(stderr, "Unrecognized command %s\n", command);
        retval = 1;
    } else {
        retval = (*handler)(argc - offset, argv + offset);
    }

    return retval;
}
