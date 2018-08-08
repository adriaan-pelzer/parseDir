#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "parseDir.h"

typedef struct file_list {
    char name[256];
    time_t mtime;
    struct file_list *next;
} file_list_t;

void file_list_free ( file_list_t *file_list ) {
    file_list_t *fe = file_list;

    while ( fe ) {
        file_list_t *next = fe->next;
        free ( fe );
        fe = next;
    }
}

file_list_t *insert_into_file_list ( file_list_t *file_list, struct stat stat_buf, const char *name ) {
    file_list_t *rc = file_list, *file_entry = NULL, *prev = NULL, *next = NULL;

    if ( ( file_entry = calloc ( 1, sizeof ( file_list_t ) ) ) == NULL )
        return rc;

    snprintf ( file_entry->name, 256, "%s", name );
    file_entry->mtime = stat_buf.st_mtime;

    next = rc;
    while ( next ) {
        if ( next->mtime > file_entry->mtime ) {
            file_entry->next = next;
            if ( prev )
                prev->next = file_entry;
            else
                rc = file_entry;
        }
        prev = next;
        next = prev->next;
    }

    if ( rc == NULL )
        rc = file_entry;

    return rc;
}

file_list_t *get_ordered_file_list ( const char *folder ) {
    file_list_t *rc = NULL, *_rc = NULL;
    DIR *dir = NULL;
    struct dirent *entry;

    if ( ( dir = opendir ( folder ) ) == NULL ) {
        goto over;
    }

    errno = 0;

    while ( ( entry = readdir ( dir ) ) ) {
        struct stat stat_buf;
        char path[256];

        snprintf ( path, 256, "%s/%s", folder, entry->d_name );

        if ( stat ( path, &stat_buf ) != 0 )
            goto over;

        if ( S_ISREG ( stat_buf.st_mode ) != 0 )
            _rc = insert_into_file_list ( _rc, stat_buf, path );
    }

    if ( errno )
        goto over;

    rc = _rc;
over:
    if ( dir )
        closedir ( dir );
    if ( rc != _rc && _rc != NULL )
        file_list_free ( _rc );
    return rc;
}

FILE *open_lock_oldest_unlocked_file ( const char *folder ) {
    FILE *rc = NULL, *_rc = NULL;
    file_list_t *ordered_file_list = NULL, *file_entry = NULL;
    char found = 0;

    if ( ( ordered_file_list = get_ordered_file_list ( folder ) ) == NULL )
        goto over;

    file_entry = ordered_file_list;

    while ( file_entry ) {
        if ( ( _rc = fopen ( file_entry->name, "r" ) ) == NULL )
            goto over;

        if ( flock ( fileno ( _rc ), LOCK_EX | LOCK_NB ) == 0 ) {
            found = 1;
            break;
        }

        file_entry = file_entry->next;
    }

    if ( ! found )
        goto over;

    rc = _rc;
over:
    if ( rc != _rc && _rc != NULL )
        fclose ( _rc );

    if ( ordered_file_list )
        file_list_free ( ordered_file_list );

    return rc;
}

int close_unlock_file ( FILE *fp ) {
    if ( flock ( fileno ( fp ), LOCK_UN ) != 0 )
        return EXIT_FAILURE;
    if ( fclose ( fp ) != 0 )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int get_filename ( FILE *fp, char **filename ) {
    char path[256];

    snprintf ( path, 256, "/proc/self/fd/%d", fileno ( fp ) );
    memset ( *filename, 0, 256 );

    if ( readlink ( path, *filename, 256 ) < 0 )
        return EXIT_FAILURE;

    return EXIT_SUCCESS
}

int close_unlink_file ( FILE *fp ) {
    char path[256], filename[256];

    if ( get_filename ( fp, &filename ) != EXIT_SUCCESS )
        return EXIT_FAILURE;

    if ( fclose ( fp ) != 0 )
        return EXIT_FAILURE;

    if ( unlink ( filename ) != 0 )
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
