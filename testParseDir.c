#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <parseDir.h>

int main ( int argc, char **argv ) {
    int rc = EXIT_FAILURE;
    char *dirname = NULL;
    fileCtx_t *oldest = NULL;

    if ( argc < 2 ) {
        fprintf ( stderr, "Usage: %s folder\n", argv[0] );
        goto over;
    }

    errno = 0;
    dirname = argv[1];

    while ( ( oldest = open_lock_oldest_unlocked_file ( dirname ) ) ) {
        char *content = NULL;

        if ( ( content = get_file_content ( oldest ) ) == NULL ) {
            fprintf ( stderr, "Cannot get file content: %s\n", strerror ( errno ) );
            goto over;
        }

        printf ( "%s: %s", oldest->fn, content );
        free ( content );

        if ( close_unlock_file ( oldest ) != EXIT_SUCCESS ) {
            fprintf ( stderr, "Cannot close and unlock file: %s\n", strerror ( errno ) );
            goto over;
        }

        oldest = NULL;

        if ( ( oldest = open_lock_oldest_unlocked_file ( dirname ) ) == NULL ) {
            fprintf ( stderr, "Cannot re-open oldest file: %s\n", strerror ( errno ) );
            goto over;
        }

        if ( ( content = get_file_content ( oldest ) ) == NULL ) {
            fprintf ( stderr, "Cannot get file content: %s\n", strerror ( errno ) );
            goto over;
        }

        printf ( "%s: %s", oldest->fn, content );
        free ( content );

        if ( close_unlink_file ( oldest ) != EXIT_SUCCESS ) {
            fprintf ( stderr, "Cannot close and unlink file: %s\n", strerror ( errno ) );
            goto over;
        }
    }

    if ( errno ) {
        fprintf ( stderr, "%s\n", strerror ( errno ) );
        goto over;
    }

    rc = EXIT_SUCCESS;
over:
    return rc;
}
