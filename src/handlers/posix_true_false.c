/**********************************************************************
 true shall always return with a 0 exit status
 false shall always return with a non-zero exit status
 **********************************************************************
 */

#include "handlers.h"

int posix_true(int argc, char **argv)
{
    return 0;
}

int posix_false(int argc, char **argv)
{
    return 1;
}

