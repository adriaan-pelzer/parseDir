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
        char content[1024];

        memset ( content, 0, 1024 );
        fread ( content, 1024, 1, oldest->fp );
        printf ( "%s: %s", oldest->fn, content );

        if ( close_unlock_file ( oldest ) != EXIT_SUCCESS ) {
            fprintf ( stderr, "Cannot close and unlock file: %s\n", strerror ( errno ) );
            goto over;
        }

        oldest = NULL;

        if ( ( oldest = open_lock_oldest_unlocked_file ( dirname ) ) == NULL ) {
            fprintf ( stderr, "Cannot re-open oldest file: %s\n", strerror ( errno ) );
            goto over;
        }

        memset ( content, 0, 1024 );
        fread ( content, 1024, 1, oldest->fp );
        printf ( "%s: %s", oldest->fn, content );

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
