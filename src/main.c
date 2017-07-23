#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "handlers.h"

typedef int (*handler_function)(int, char**);

struct command_map {
    char *command;
    handler_function handler;
};

const struct command_map map[] = {
    { "cat",    posix_cat       },
    { NULL,     NULL            }
};

/*
 * Main function which dispatches the execution to different handlers
 * depending on the value of argv[0]
 */
int main(int argc, char **argv)
{
    const struct command_map *map_entry;
    int retval = 0;
    int command_found = 0;
    char *command;

    /* Get the basename of the command */
    command = strrchr(argv[0], '/');
    if (command == NULL) {
        command = argv[0];
    } else {
        /* Skip over the / */
        command++;
    }

    for (map_entry = &map[0]; map_entry->command; map_entry++) {
        if (strcmp(map_entry->command, command) == 0) {
            retval = (*(map_entry->handler))(argc, argv);
            command_found = 1;
            break;
        }
    }

    if (!command_found) {
        fprintf(stderr, "Unrecognized command %s\n", command);
        retval = 1;
    }

    return retval;
}
